#ifndef __BSP_RNG_H__
#define __BSP_RNG_H__

#include "stm32f4xx_hal.h"

uint8_t BSP_RNG_Init(void);
int BSP_RNG_GetRandomNumber(void);
int BSP_RNG_GetRandomNumberInRange(int min, int max);

#endif // !__BSP_RNG_H__