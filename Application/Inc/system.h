#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "stm32f4xx_hal.h"

void System_NVIC_SetVectorTable(uint32_t baseAddress, uint32_t offset);

#endif // !__SYSTEM_H__