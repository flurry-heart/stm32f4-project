#ifndef __BSP_DAC_H__
#define __BSP_DAC_H__

#include <math.h>

#include "stm32f4xx_hal.h"

#include "bsp_systick.h"

extern DAC_HandleTypeDef g_dac_handle;

extern uint16_t g_dac_sin_array[4096];

void BSP_DAC_Init(void);
void BSP_DAC_ConfigChannel(DAC_HandleTypeDef *hdac, uint32_t DAC_Trigger, uint32_t DAC_OutputBuffer, uint32_t channel);
void BSP_DAC_SetVoltage(DAC_HandleTypeDef *hdac, uint32_t channel, double voltage);
void BSP_DAC_OutputTriangularWave(DAC_HandleTypeDef *hdac, uint32_t channel, double maxVoltage, uint16_t sampleInterval, uint16_t samples, uint16_t n);

void BSP_DAC_CreateSinArray(double maxVoltage, uint16_t samples);

#endif // !__BSP_DAC_H__