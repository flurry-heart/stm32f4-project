#ifndef __BSP_SYSTICK_H__
#define __BSP_SYSTICK_H__

#include "stm32f4xx_hal.h"

void Delay_Init(void);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);

#endif // !__BSP_SYSTICK_H__