#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "stm32f4xx_hal.h"

#define ADC_OVERSAMPLE_TIMES    256                                             // ADC过采样次数，从12位提高到16位需要采样256次
#define ADC_DMA_BUFFER_SIZE     ADC_OVERSAMPLE_TIMES * 10                       // 累计过采样10次

extern ADC_HandleTypeDef g_adc1_handle;
extern uint16_t g_adc_dma_buffer[ADC_DMA_BUFFER_SIZE];

void BSP_ADC_Init(ADC_HandleTypeDef *hadc, ADC_TypeDef *ADCx,uint32_t numOfConversion, FunctionalState continuousConvMode, FunctionalState dmaContinuousRequests);
void BSP_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t rank, uint32_t samplingTime);
void BSP_ADC_DMA_Enable(ADC_HandleTypeDef *hadc, DMA_HandleTypeDef *hdma, uint16_t ndtr);

#endif // !__BSP_ADC_H__