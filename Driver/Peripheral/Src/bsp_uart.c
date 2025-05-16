#include "bsp_uart.h"

UART_HandleTypeDef g_usart1_handle;                                             // USART1句柄
UART_HandleTypeDef g_usart2_handle;                                             // USART2句柄

UART_FrameData_t g_usart1_frame_data;                                           // USART1帧数据
UART_FrameData_t g_usart2_frame_data;                                           // USART2帧数据

static void BSP_UART_IRQHandler(UART_HandleTypeDef *huart, UART_FrameData_t *pFrameData);

/**
 * @brief 串口初始化函数
 * 
 * @param huart 串口句柄
 * @param UARTx 串口寄存器基地址
 * @param band 波特率
 */
void BSP_UART_Init(UART_HandleTypeDef *huart, USART_TypeDef *UARTx, uint32_t band)
{
    huart->Instance = UARTx;                                                    // 寄存器基地址
    huart->Init.BaudRate = band;                                                // 波特率
    huart->Init.WordLength = UART_WORDLENGTH_8B;                                // 数据位
    huart->Init.StopBits = UART_STOPBITS_1;                                     // 停止位
    huart->Init.Parity = UART_PARITY_NONE;                                      // 奇偶校验位
    huart->Init.Mode = UART_MODE_TX_RX;                                         // 收发模式
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;                                // 硬件流控制
    huart->Init.OverSampling = UART_OVERSAMPLING_16;                            // 过采样
    HAL_UART_Init(huart);

    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);                                  // 使能USART接收中断
    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);                                  // 使能USART总线空闲中断
}

/**
 * @brief 串口底层初始化函数
 * 
 * @param huart 串口句柄
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (huart->Instance == USART1)                                              // 初始化的串口是否是USART1
    {
        __HAL_RCC_USART1_CLK_ENABLE();                                          // 使能USART1时钟
        __HAL_RCC_GPIOA_CLK_ENABLE();                                           // 使能对应GPIO的时钟

        // PA9 -> USART TXD
        GPIO_InitStruct.Pin = GPIO_PIN_9;                                       // USART1 TXD的引脚
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                                 // 推挽式复用
        GPIO_InitStruct.Pull = GPIO_NOPULL;                                     // 不使用上下拉
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                           // 输出速度
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;                            // 复用功能
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // PA10 -> USART RXD
        GPIO_InitStruct.Pin = GPIO_PIN_10;                                      // USART1 RXD的引脚
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(USART1_IRQn);                                        // 使能USART1中断
        HAL_NVIC_SetPriority(USART1_IRQn, 4, 0);                                // 设置中断优先级
    }
    else if (huart->Instance == USART2)                                         // 初始化的串口是否是USART2
    {
        __HAL_RCC_USART2_CLK_ENABLE();                                          // 使能USART2时钟
        __HAL_RCC_GPIOA_CLK_ENABLE();                                           // 使能对应GPIO的时钟

        // PA2 -> USART2 TXD
        GPIO_InitStruct.Pin = GPIO_PIN_2;                                       // USART2 TXD的引脚
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                                 // 推挽式复用
        GPIO_InitStruct.Pull = GPIO_NOPULL;                                     // 不使用上下拉
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                           // 输出速度
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;                            // 复用功能
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // PA3 -> USART2 RXD
        GPIO_InitStruct.Pin = GPIO_PIN_3;                                       // USART2 RXD的引脚
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(USART2_IRQn);                                        // 使能USART2中断
        HAL_NVIC_SetPriority(USART2_IRQn, 4, 0);                                // 设置中断优先级
    }
}

/**
 * @brief 获取串口接收到的数据帧的有效长度函数
 * 
 * @param frameData 串口接收的数据帧
 * @return uint16_t 串口接收到的数据帧的有效长度
 */
uint16_t BSP_UART_GetFrameDataLength(UART_FrameData_t *frameData)
{
    return (frameData->finsh) ? frameData->length : 0;
}

/**
 * @brief 清除串口接收到的数据帧函数
 * 
 * @param frameData 串口接收的数据帧
 */
void BSP_UART_ClearFrameData(UART_FrameData_t *frameData)
{
    memset(frameData->data, 0, frameData->length);
    frameData->length = 0;
    frameData->finsh = false;
}

/**
 * @brief USART1中断服务函数
 * 
 */
void USART1_IRQHandler(void)
{
    g_usart1_frame_data.finsh = false;
    BSP_UART_IRQHandler(&g_usart1_handle, &g_usart1_frame_data);
}

/**
 * @brief USART2中断服务函数
 * 
 */
void USART2_IRQHandler(void)
{
    g_usart2_frame_data.finsh = false;
    BSP_UART_IRQHandler(&g_usart2_handle, &g_usart2_frame_data);
}

/**
 * @brief UART中断服务函数
 * 
 * @param huart 串口句柄
 * @param pFrameData 串口接收的数据帧
 */
static void BSP_UART_IRQHandler(UART_HandleTypeDef *huart, UART_FrameData_t *pFrameData)
{
    uint8_t temp = 0;
  
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) != RESET)                     // USART接收过载错误中
    {
        __HAL_UART_CLEAR_OREFLAG(huart);                                        // 清除接收过载错误中断标志
        (void)huart->Instance->SR;                                              // 先读SR寄存器，再读DR寄存器 
        (void)huart->Instance->DR;
    }
  
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) != RESET)                    // UART接收中断
    {
        HAL_UART_Receive(huart, &temp, 1, HAL_MAX_DELAY);                       // UART接收数据
  
        if (pFrameData->length < (UART_RECEIVE_LENGTH - 1))                     // 判断USART接收缓冲是否溢出，留出一位给结束符'\0'
        {
            pFrameData->data[pFrameData->length] = temp;                        // 将接收到的数据写入缓冲
            pFrameData->length++;                                               // 更新接收到的数据长度
        }
        else                                                                    // UART接收缓冲溢出
        {
            pFrameData->length = 0;                                             // 覆盖之前收到的数据
            pFrameData->data[pFrameData->length] = temp;                        // 将接收到的数据写入缓冲
            pFrameData->length++;                                               // 更新接收到的数据长度
        }
    }
  
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)                    // UART总线空闲中断
    {
        pFrameData->finsh = true;                                               // 标记帧接收完成
        pFrameData->data[pFrameData->length] = '\0';                            // 添加结束符
        __HAL_UART_CLEAR_IDLEFLAG(huart);                                       // 清除USART总线空闲中断
    }
}

/**
 * @brief 重写_write使用printf()函数
 * 
 * @param fd 一个非负整数，代表要写入数据的文件或设备的标识
 * @param ptr 一个指向字符数据的指针，即要写入的数据的起始位置
 * @param length 一个整数，表示要写入的数据的字节数
 * @return int 数据的字节数
 */
int _write(int fd, char *ptr, int length)
{
    HAL_UART_Transmit(&g_usart1_handle, (uint8_t *)ptr, length, 0xFFFF);        // g_usart1_handle是对应串口
    return length;
}

/**
 * @brief 多串口使用printf()函数
 * 
 * @param huart 串口句柄
 * @param fmt 格式化字符串
 * @param ... 格式化参数
 */
void BSP_UART_Printf(UART_HandleTypeDef *huart, char *fmt, ...)
{
    char buffer[UART_RECEIVE_LENGTH + 1];                                       // 用来存放转换后的数据
    uint16_t i = 0;

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, UART_RECEIVE_LENGTH + 1, fmt, args);                      // 将格式化字符串转换为字符数组

    i = (strlen(buffer) > UART_RECEIVE_LENGTH) ? UART_RECEIVE_LENGTH : strlen(buffer);

    HAL_UART_Transmit(huart, (uint8_t *)buffer, i, 1000);                         // 串口发送数据

    va_end(args);
}