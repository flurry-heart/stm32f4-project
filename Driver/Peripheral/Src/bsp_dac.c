#include "bsp_dac.h"

DAC_HandleTypeDef g_dac_handle;

uint16_t g_dac_sin_array[4096];

/**
 * @brief DAC初始化
 * 
 */
void BSP_DAC_Init(void)
{
    g_dac_handle.Instance = DAC;                                                // DAC寄存器基地址
    HAL_DAC_Init(&g_dac_handle);
}

/**
 * @brief DAC底层初始化函数
 * 
 * @param hdac DAC句柄
 */
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hdac->Instance == DAC)
    {
        __HAL_RCC_DAC_CLK_ENABLE();                                             // 使能DAC时钟
        __HAL_RCC_GPIOA_CLK_ENABLE();                                           // 使能GPIOA时钟

        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/**
 * @brief DAC通道配置函数
 * 
 * @param hdac DAC句柄
 * @param DAC_Trigger 触发选择，可选值：[DAC_TRIGGER_NONE, DAC_TRIGGER_Tx_TRGO(x可取[2, 4, 5, 5, 6, 7, 8]), DAC_TRIGGER_EXT_IT9, DAC_TRIGGER_SOFTWARE]
 * @param DAC_OutputBuffer 输出缓冲，可选值：[DAC_OUTPUTBUFFER_ENABLE, DAC_OUTPUTBUFFER_DISABLE]
 * @param channel 通道，可选值：[DAC_CHANNEL_1, DAC_CHANNEL_2]
 */
void BSP_DAC_ConfigChannel(DAC_HandleTypeDef *hdac, uint32_t trigger, uint32_t outputBuffer, uint32_t channel)
{
    DAC_ChannelConfTypeDef DAC_ChannelConfStruct = {0};

    DAC_ChannelConfStruct.DAC_Trigger = trigger;
    DAC_ChannelConfStruct.DAC_OutputBuffer = outputBuffer;
    HAL_DAC_ConfigChannel(hdac, &DAC_ChannelConfStruct, channel);
}

/**
 * @brief DAC设置输出电压函数
 * 
 * @param hdac DAC句柄
 * @param Channel 通道，可选值：[DAC_CHANNEL_1, DAC_CHANNEL_2]
 * @param voltage 电压值
 */
void BSP_DAC_SetVoltage(DAC_HandleTypeDef *hdac, uint32_t channel, double voltage)
{
    voltage = voltage * 4096 / 3.3;
    voltage = (voltage > 4095) ? 4095 : voltage;
    HAL_DAC_SetValue(hdac, channel, DAC_ALIGN_12B_R, voltage);
}

/**
 * @brief DAC输出三角波函数
 * 
 * @param hdac DAC句柄
 * @param Channel 通道，可选值：[DAC_CHANNEL_1, DAC_CHANNEL_2]
 * @param maxVoltage 最大电压值
 * @param sampleInterval 采样点间隔
 * @param samples 采样点个数
 * @param n 波形个数
 */
void BSP_DAC_OutputTriangularWave(DAC_HandleTypeDef *hdac, uint32_t channel, double maxVoltage, uint16_t sampleInterval, uint16_t samples, uint16_t n)
{
    uint16_t i = 0, j = 0;
    double increment = 0, currentValue = 0;
    int maxValue = maxVoltage * 4096 / 3.3;
  
    if((maxValue + 1) <= samples)                                               // 数据不合法
    {   
        return;
    }

    increment = (maxValue + 1) / (samples / 2);                                 // 计算递增量

    for(j = 0; j < n; j++)
    {
        HAL_DAC_SetValue(hdac, channel, DAC_ALIGN_12B_R, currentValue);
        for(i = 0; i < (samples / 2); i++)                                      // 输出上升沿
        {
            currentValue += increment;                                          // 新的输出值
            HAL_DAC_SetValue(hdac, channel, DAC_ALIGN_12B_R, currentValue);
            Delay_us(sampleInterval);
        }
        for(i = 0; i < (samples / 2); i++)                                      // 输出下降沿
        {
            currentValue -= increment;                                          // 新的输出值
            HAL_DAC_SetValue(hdac, channel, DAC_ALIGN_12B_R, currentValue);
            Delay_us(sampleInterval);
        }
    }
}

/**
 * @brief 生成DAC输出正弦波的数组函数
 * 
 * @param maxVoltage 最大值
 * @param samples 采样点数
 */
void BSP_DAC_CreateSinArray(double maxVoltage, uint16_t samples)
{
    int maxValue = maxVoltage * 4096 / 3.3;
    double outputData = 0;                                                      // 存放计算后的数字量
    double increment = (2 * 3.141592653589793) / samples;                       // 计算相邻两个点的x轴间隔

    if (maxValue <= (samples / 2))                                              // 数据不合法
    {
        return;
    }
  
    for (uint8_t i = 0; i < samples; i++)
    {
        // 正弦波解析式：y=Asin(ωx+Φ)+b
        outputData = maxValue * sin(i * increment) + maxValue;
        outputData = (outputData > 4095) ? 4095 : outputData;
        g_dac_sin_array[i] = outputData;
    }
}