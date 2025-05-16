#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "stm32f4xx_hal.h"

#define ADDRESS_FLASH_SECTOR_0  ((uint32_t )0x08000000)                         // 扇区 0 起始地址，16 Kbyte
#define ADDRESS_FLASH_SECTOR_1  ((uint32_t )0x08004000)                         // 扇区 1 起始地址，16 Kbyte
#define ADDRESS_FLASH_SECTOR_2  ((uint32_t )0x08008000)                         // 扇区 2 起始地址，16 Kbyte
#define ADDRESS_FLASH_SECTOR_3  ((uint32_t )0x0800C000)                         // 扇区 3 起始地址，16 Kbyte
#define ADDRESS_FLASH_SECTOR_4  ((uint32_t )0x08010000)                         // 扇区 4 起始地址，64 Kbyte
#define ADDRESS_FLASH_SECTOR_5  ((uint32_t )0x08020000)                         // 扇区 5 起始地址，128 Kbyte
#define ADDRESS_FLASH_SECTOR_6  ((uint32_t )0x08040000)                         // 扇区 6 起始地址，128 Kbyte
#define ADDRESS_FLASH_SECTOR_7  ((uint32_t )0x08060000)                         // 扇区 7 起始地址，128 Kbyte
#define ADDRESS_FLASH_SECTOR_8  ((uint32_t )0x08080000)                         // 扇区 8 起始地址，128 Kbyte
#define ADDRESS_FLASH_SECTOR_9  ((uint32_t )0x080A0000)                         // 扇区 9 起始地址，128 Kbyte
#define ADDRESS_FLASH_SECTOR_10 ((uint32_t )0x080C0000)                         // 扇区 10 起始地址，128 Kbyte
#define ADDRESS_FLASH_SECTOR_11 ((uint32_t )0x080E0000)                         // 扇区 11 起始地址，128 Kbyte

#define FLASH_WAITE_TIME        50000                                           // FLASH 等待超时时间

void BSP_FLASH_ReadData(uint32_t address, uint32_t *data, uint32_t length);
void BSP_FLASH_WriteData(uint32_t address, uint32_t *data, uint32_t length);

#endif // !__BSP_FLASH_H__