#include "bsp_can.h"

CAN_HandleTypeDef g_can1_handle;

/**
 * @brief CAN初始化函数
 * 
 * @param mode CAN的工作模式
 * @param BKP 分频值
 * @param TS1 时间段1
 * @param TS2 时间段2
 * @param sjw 重新同步跳跃宽度
 */
void BSP_CAN_Init(CAN_HandleTypeDef *hcan, CAN_TypeDef *CANx, uint32_t mode, uint32_t BKP, uint32_t TS1, uint32_t TS2, uint32_t sjw)
{
    hcan->Instance = CANx;                                                      // CAN的寄存器基地址
    hcan->Init.Mode = mode;                                                     // CAN工作模式

    // 波特率相关
    hcan->Init.Prescaler = BKP + 1;                                             // 分频系数
    hcan->Init.TimeSeg1 = TS1;                                                  // 时间段1
    hcan->Init.TimeSeg2 = TS2;                                                  // 时间段2
    hcan->Init.SyncJumpWidth = sjw;                                             // 重新同步跳跃宽度

    // CAN功能设置
    hcan->Init.AutoBusOff = DISABLE;                                            // 禁止自动离线管理
    hcan->Init.AutoRetransmission = DISABLE;                                    // 禁止自动重发
    hcan->Init.AutoWakeUp = DISABLE;                                            // 禁止自动唤醒
    hcan->Init.ReceiveFifoLocked = DISABLE;                                     // 禁止自动接收FIFO锁定
    hcan->Init.TimeTriggeredMode = DISABLE;                                     // 禁止时间触发通信模式
    hcan->Init.TransmitFifoPriority = DISABLE;                                  // 禁止发送FIFO优先级
  
    HAL_CAN_Init(hcan);
}

/**
 * @brief CAN底层初始化函数
 * 
 * @param hcan CAN句柄
 */
void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hcan->Instance == CAN1)
    {
        __HAL_RCC_CAN1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/**
 * @brief CAN配置过滤器函数
 * 
 * @param hcan CAN句柄
 * @param filterMode 过滤器模式，可选值: [CAN_FILTERMODE_IDMASK, CAN_FILTERMODE_IDLIST]
 * @param filterScale 过滤器位宽，可选值: [CAN_FILTERSCALE_16BIT, CAN_FILTERSCALE_32BIT]
 * @param filterId 过滤器标识符
 * @param filterMaskId 过滤器掩码号
 * @param bank 过滤器组
 * @param fifo FIFO选择，可选值: [CAN_RX_FIFO0, CAN_RX_FIFO1]
 */
void BSP_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, uint32_t mode, uint32_t scale, uint32_t id, uint32_t maskId, uint32_t bank, uint32_t fifo)
{
    CAN_FilterTypeDef CAN_FilterStruct = {0};

    CAN_FilterStruct.FilterMode = mode;                                         // 过滤器的模式
    CAN_FilterStruct.FilterScale = scale;                                       // 过滤器的位宽

    CAN_FilterStruct.FilterIdHigh = id >> 16;                                   // 过滤器标识符高位
    CAN_FilterStruct.FilterIdLow = id & 0xFFFF;                                 // 过滤器标识符低位
    CAN_FilterStruct.FilterMaskIdHigh = maskId >> 16;                           // 过滤器掩码号高
    CAN_FilterStruct.FilterMaskIdLow = maskId & 0xFFFF;                         // 过滤器掩码号低

    CAN_FilterStruct.FilterBank = bank;                                         // 指定过滤器组
    CAN_FilterStruct.FilterFIFOAssignment = fifo;                               // 指定过滤器分配到哪个FIFO
    CAN_FilterStruct.FilterActivation = CAN_FILTER_ENABLE;                      // 禁用或者使能过滤器

    HAL_CAN_ConfigFilter(hcan, &CAN_FilterStruct);
}

/**
 * @brief CAN发送数据函数
 * 
 * @param hcan CAN句柄
 * @param id 标识符
 * @param mail 发送数据的邮箱，可选值: CAN_TX_MAILBOXx, x可选[0, 1, 2]
 * @param data 数据
 * @param length 数据长度
 */
void BSP_CAN_SendMessage(CAN_HandleTypeDef *hcan, uint32_t id, uint32_t mail, uint8_t *data, uint8_t length)
{
    CAN_TxHeaderTypeDef CAN_TxHeaderStruct = {0};
    
    CAN_TxHeaderStruct.StdId = id;                                              // 标准标识符
    CAN_TxHeaderStruct.DLC = length;                                            // 数据长度
    CAN_TxHeaderStruct.IDE = CAN_ID_STD;                                        // 标识符类型
    CAN_TxHeaderStruct.RTR = CAN_RTR_DATA;                                      // 数据类型
    HAL_CAN_AddTxMessage(hcan, &CAN_TxHeaderStruct, data, &mail);

    while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3);
}

/**
 * @brief CAN接收数据
 * 
 * @param hcan CAN句柄
 * @param fifo FIFO选择，可选值: CAN_RX_FIFOx, x可取[0, 1]
 * @param data 保存接收的数据
 * @return uint8_t 接收数据的长度
 */
uint8_t BSP_CAN_ReceiveMessage(CAN_HandleTypeDef *hcan, uint32_t fifo, uint8_t *data)
{
    CAN_RxHeaderTypeDef CAN_RxHeaderStruct = {0};

    if (HAL_CAN_GetRxFifoFillLevel(hcan, fifo) == 0)
    {
        return 0;
    }
    HAL_CAN_GetRxMessage(hcan, fifo, &CAN_RxHeaderStruct, data);

    return CAN_RxHeaderStruct.DLC;
}