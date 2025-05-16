#include "bsp_adc.h"

ADC_HandleTypeDef g_adc1_handle;

uint16_t g_adc_dma_buffer[ADC_DMA_BUFFER_SIZE];

/**
 * @brief ADC初始化函数
 * 
 * @param hadc ADC句柄
 * @param ADCx ADC寄存器基地址
 * @param numOfConversion 常规转换的通道数
 * @param continuousConvMode ADC是否连续转换， ENABLE:连续转换, DISABLE:单次转换
 * @param dmaContinuousRequests DMA是否连续请求， ENABLE:使用DMA, DISABLE:不使用DMA
 */
void BSP_ADC_Init(ADC_HandleTypeDef *hadc, ADC_TypeDef *ADCx,uint32_t numOfConversion, FunctionalState continuousConvMode, FunctionalState dmaContinuousRequests)
{
    hadc->Instance = ADCx;                                                      // ADC寄存器基地址
    hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;                       // ADC时钟分频因子
    hadc->Init.Resolution = ADC_RESOLUTION_12B;                                 // 12位模式
    hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;                                 // 数据右对齐
    hadc->Init.EOCSelection = DISABLE;                                          // 转换完成标志位
    hadc->Init.ScanConvMode = ENABLE;                                           // 配置是否使用扫描
    hadc->Init.ContinuousConvMode = continuousConvMode;                         // ADC是否自动连续转换
    hadc->Init.NbrOfConversion = numOfConversion;                               // 配置常规转换通道
    hadc->Init.DiscontinuousConvMode = DISABLE;                                 // 是否使用不连续的采样模式   
    hadc->Init.NbrOfDiscConversion = 0;                                         // 不连续采样通道数  
    hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;                           // 外部触发方式的选择
    hadc->Init.DMAContinuousRequests = dmaContinuousRequests;                   // 是否开启DMA连续转换
    HAL_ADC_Init(hadc);
}

/**
 * @brief ADC底层初始化函数
 * 
 * @param hadc ADC句柄
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hadc->Instance == ADC1)
    {
        __HAL_RCC_ADC1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_5;                                       // PA5引脚设置为ADC1的通道5引脚
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;                                // 模拟功能
        GPIO_InitStruct.Pull = GPIO_NOPULL;                                     // 不使用上下拉
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/**
 * @brief ADC通道配置函数
 * 
 * @param hadc ADC句柄
 * @param channel ADC通道，可选值: ADC_CHANNEL_0 ~ ADC_CHANNEL_18
 * @param rank 采样序列
 * @param samplingTime 采样时间，可选值：ADC_SAMPLETIME_xCYCLES，x: [3,15,28,56,112,144,480]
 */
void BSP_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t rank, uint32_t samplingTime)
{
    ADC_ChannelConfTypeDef ADC_ChannelConfStruct = {0};

    ADC_ChannelConfStruct.Channel = channel;                                    // ADC通道
    ADC_ChannelConfStruct.Rank = rank;                                          // 采样序列
    ADC_ChannelConfStruct.SamplingTime = samplingTime;                          // 采样时间
    HAL_ADC_ConfigChannel(hadc, &ADC_ChannelConfStruct);
}

/**
 * @brief 使能一次ADC的DMA传输函数
 * 
 * @param hadc ADC句柄
 * @param hdma DMA句柄
 * @param ndtr DMA传输的次数
 */
void BSP_ADC_DMA_Enable(ADC_HandleTypeDef *hadc, DMA_HandleTypeDef *hdma, uint16_t ndtr) 
{
    __HAL_ADC_DISABLE(hadc);                                                    // 先关闭ADC
    __HAL_DMA_DISABLE(hdma);                                                    // 关闭DMA传输
    hdma->Instance->NDTR= ndtr;                                                 // 重设DMA传输数据量 
    __HAL_DMA_ENABLE(hdma);                                                     // 开启DMA传输
    __HAL_ADC_ENABLE(hadc);                                                     // 重新启动ADC
    hadc->Instance->CR2 |= 1 << 30;                                             // 启动规则转换通道
}