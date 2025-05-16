#ifndef __BSP_IAP_H__
#define __BSP_IAP_H__

#include <stdio.h>

#include "stm32f4xx_hal.h"

#include "bsp_flash.h"

#define FLASH_APP1_ADDRESS         0x08010000                                   // 第一个应用程序起始地址（存放在内部FLASH）

void BSP_IAP_WriteAppBin(uint32_t address, uint8_t *data, uint32_t length);
void BSP_IAP_LoadApp(uint32_t address);

#endif // !__BSP_IAP_H__