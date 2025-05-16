#ifndef __BSP_WWDG_H__
#define __BSP_WWDG_H__

#include "stm32f4xx_hal.h"

extern WWDG_HandleTypeDef g_wwdg_handle;

void BSP_WWDG_Init(uint16_t prescaler, uint8_t count, uint8_t window);

#endif // !__BSP_WWDG_H__