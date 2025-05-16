#ifndef __BSP_IWDG_H__
#define __BSP_IWDG_H__

#include "stm32f4xx_hal.h"

extern IWDG_HandleTypeDef g_iwdg_handle;

void BSP_IWDG_Init(uint8_t prescaler, uint16_t reload);

#endif // !__BSP_IWDG_H__