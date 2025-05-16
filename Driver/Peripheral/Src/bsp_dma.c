#include "bsp_dma.h"

DMA_HandleTypeDef g_dma1_handle;
DMA_HandleTypeDef g_dma2_handle;
uint8_t dma2_stream0_is_finished;

/**
 * @brief DMA内存到内存间的数据传输初始化函数
 * 
 * @param hdma  DMA句柄
 * @param dma_stream DMA数据流，可选值: DMA2_Stream0 ~ DMA2_Stream7
 * @param channel DMA通道，可选值: DMA_CHANNEL_0 ~ DMA_CHANNEL_7
 * @param dataLength 数据长度，可选值: [8, 16, 32]
 * @param mode DMA模式，可选值: [DMA_NORMAL, DMA_CIRCULAR, DMA_PFCTRL]
 * @param priority DMA通道优先级，可选值: [DMA_PRIORITY_LOW, DMA_PRIORITY_MEDIUM, DMA_PRIORITY_HIGH, DMA_PRIORITY_VERY_HIGH]
 */
void BSP_DMA_MemoryToMemory_Init(DMA_HandleTypeDef *hdma, DMA_Stream_TypeDef *dma_stream, uint32_t channel, uint8_t dataLength, uint32_t mode, uint32_t priority)
{
    __HAL_RCC_DMA2_CLK_ENABLE();

    hdma->Instance = dma_stream;                                                // DMA寄存器基地址
    hdma->Init.Channel = channel;                                               // DMA通道
    hdma->Init.Direction = DMA_MEMORY_TO_MEMORY;                                // 存储器到存储器
    hdma->Init.PeriphInc = DMA_PINC_ENABLE;                                     // 外设地址增模式
    hdma->Init.MemInc = DMA_MINC_ENABLE;                                        // 存储器地址增模式
    switch (dataLength)
    {
    case 8:
        hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;                   // 外设地址长度
        hdma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;                      // 存储器地址长度
        break;
    case 16:
        hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;               // 外设地址长度
        hdma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;                  // 存储器地址长度
        break;
    case 32:
        hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;                   // 外设地址长度
        hdma->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;                      // 存储器地址长度
        break;
    default:
        break;
    }
  
    hdma->Init.Mode = mode;                                                     // DMA模式
    hdma->Init.Priority = priority;                                             // DMA通道优先级
    HAL_DMA_Init(hdma);

    if (dma_stream == DMA2_Stream0)
    {
        HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 4, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    }
}

/**
 * @brief DMA存储器到外设间的数据传输初始化函数
 * 
 * @param hdma DMA句柄
 * @param dma_stream DMA数据流，可选值: DMA1_Stream0 ~ DMA1_Stream7, DMA2_Stream0 ~ DMA2_Stream7
 * @param channel DMA通道，可选值: DMA_CHANNEL_0 ~ DMA_CHANNEL_7
 * @param dataLength 数据长度，可选值: [8, 16, 32]
 * @param mode DMA模式，可选值: [DMA_NORMAL, DMA_CIRCULAR, DMA_PFCTRL]
 * @param priority DMA通道优先级，可选值: [DMA_PRIORITY_LOW, DMA_PRIORITY_MEDIUM, DMA_PRIORITY_HIGH, DMA_PRIORITY_VERY_HIGH]
 */
void BSP_DMA_MemoryToPeripheral_Init(DMA_HandleTypeDef *hdma, DMA_Stream_TypeDef *dma_stream, uint32_t channel, uint8_t dataLength, uint32_t mode, uint32_t priority)
{ 
    if ((uint32_t)dma_stream > (uint32_t)DMA2)                                  // 得到当前stream是属于DMA2还是DMA1
    {
        __HAL_RCC_DMA2_CLK_ENABLE();
    }
    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE();
    }

    hdma->Instance = dma_stream;                                                // 数据流选择
    hdma->Init.Channel = channel;                                               // DMA通道选择
    hdma->Init.Direction = DMA_MEMORY_TO_PERIPH;                                // 存储器到外设
    hdma->Init.PeriphInc = DMA_PINC_DISABLE;                                    // 外设非增量模式 
    hdma->Init.MemInc = DMA_MINC_ENABLE;                                        // 存储器增量模式
    switch (dataLength)
    {
    case 8:
        hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;                   // 外设地址长度
        hdma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;                      // 存储器地址长度
        break;
    case 16:
        hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;               // 外设地址长度
        hdma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;                  // 存储器地址长度
        break;
    case 32:
        hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;                   // 外设地址长度
        hdma->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;                      // 存储器地址长度
        break;
    default:
        break;
    }
    hdma->Init.Mode = mode;                                                     // DMA模式
    hdma->Init.Priority = priority;                                             // DMA通道优先级
    hdma->Init.FIFOMode = DMA_FIFOMODE_DISABLE;                                 // 关闭FIFO模式
    hdma->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;                         // FIFO阈值配置
    hdma->Init.MemBurst = DMA_MBURST_SINGLE;                                    // 存储器突发单次传输
    hdma->Init.PeriphBurst = DMA_PBURST_SINGLE;                                 // 外设突发单次传输

    HAL_DMA_Init(hdma);
}

/**
 * @brief DMA外设到存储器间的数据传输初始化函数
 * 
 * @param hdma DMA句柄
 * @param dma_stream DMA数据流，可选值: DMA1_Stream0 ~ DMA1_Stream7, DMA2_Stream0 ~ DMA2_Stream7
 * @param channel DMA通道，可选值: DMA_CHANNEL_0 ~ DMA_CHANNEL_7
 * @param dataLength 数据长度，可选值: [8, 16, 32]
 * @param mode DMA工作模式，可选值: [DMA_NORMAL, DMA_CIRCULAR, DMA_PFCTRL]
 * @param priority DMA通道优先级，可选值: [DMA_PRIORITY_LOW, DMA_PRIORITY_MEDIUM, DMA_PRIORITY_HIGH, DMA_PRIORITY_VERY_HIGH]
 */
void BSP_DMA_PeripheralToMemory_Init(DMA_HandleTypeDef *hdma, DMA_Stream_TypeDef *dma_stream, uint32_t channel, uint8_t dataLength, uint32_t mode,  uint32_t priority)
{ 
    if ((uint32_t)dma_stream > (uint32_t)DMA2)                                  // 得到当前stream是属于DMA2还是DMA1
    {
        __HAL_RCC_DMA2_CLK_ENABLE();
    }
    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE();
    }

    hdma->Instance = dma_stream;                                                // 数据流选择
    hdma->Init.Channel = channel;                                               // DMA通道选择
    hdma->Init.Direction = DMA_PERIPH_TO_MEMORY;                                // 外设到存储器
    hdma->Init.PeriphInc = DMA_PINC_DISABLE;                                    // 外设非增量模式 
    hdma->Init.MemInc = DMA_MINC_ENABLE;                                        // 存储器增量模式
    switch (dataLength)
    {
    case 8:
        hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;                   // 外设地址长度
        hdma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;                      // 存储器地址长度
        break;
    case 16:
        hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;               // 外设地址长度
        hdma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;                  // 存储器地址长度
        break;
    case 32:
        hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;                   // 外设地址长度
        hdma->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;                      // 存储器地址长度
        break;
    default:
        break;
    }
    hdma->Init.Mode = mode;                                                     // DMA模式
    hdma->Init.Priority = priority;                                             // DMA通道优先级
    hdma->Init.FIFOMode = DMA_FIFOMODE_DISABLE;                                 // 关闭FIFO模式
    hdma->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;                         // FIFO阈值配置
    hdma->Init.MemBurst = DMA_MBURST_SINGLE;                                    // 存储器突发单次传输
    hdma->Init.PeriphBurst = DMA_PBURST_SINGLE;                                 // 外设突发单次传输

    HAL_DMA_Init(hdma);
}

/**
 * @brief DMA2的Stream0中断函数
 * 
 */
void DMA2_Stream0_IRQHandler(void)
{
    if (DMA2->LISR & (1 << 5))                                                  // DMA2_Stream0传输完成标志
    {
        dma2_stream0_is_finished = 1;                                           // 标记DMA传输完成
        DMA2->LIFCR |= (1 << 5);                                                // 清除DMA2_Stream0传输完成标志
    }
}

/**
 * @brief DMA传输数据完成回调函数
 * 
 * @param hdma DMA句柄
 */
void DMA_CompleteCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma->Instance == DMA2_Stream0)
    {
        dma2_stream0_is_finished = 1;
    }
}