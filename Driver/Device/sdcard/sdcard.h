#ifndef __SDCARD_H__
#define __SDCARD_H__

#include "stm32f4xx_hal.h"

extern SD_HandleTypeDef g_sd_handler;

uint8_t SD_Init(void);
uint8_t SD_ReadData(SD_HandleTypeDef *hsd, uint32_t blockAddress, uint32_t blockCount, uint8_t *buffer);
uint8_t SD_WriteData(SD_HandleTypeDef *hsd, uint32_t blockAddress, uint32_t blockCount, uint8_t *buffer);

#endif // !__SDCARD_H__