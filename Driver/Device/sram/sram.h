#ifndef __SRAM_H__
#define __SRAM_H__

#include "stm32f4xx_hal.h"

/* SRAM基地址，根据SRAM_FSMC_NEX的设置来决定基址地址
 * 我们一般使用FSMC的块1（BANK1）来驱动SRAM, 块1地址范围总大小为256MB,均分成4块:
 * 存储块1（FSMC_NE1）地址范围: 0X6000 0000 ~ 0X63FF FFFF
 * 存储块2（FSMC_NE2）地址范围: 0X6400 0000 ~ 0X67FF FFFF
 * 存储块3（FSMC_NE3）地址范围: 0X6800 0000 ~ 0X6BFF FFFF
 * 存储块4（FSMC_NE4）地址范围: 0X6C00 0000 ~ 0X6FFF FFFF
 */
#define SRAM_FSMC_NEX           3
#define SRAM_BASE_ADDRESS       (0x60000000 + (0x4000000 * (SRAM_FSMC_NEX - 1)))

void SRAM_Init(void);
void SRAM_WriteData(uint32_t address, uint8_t *data, uint32_t length);
void SRAM_ReadData(uint32_t address, uint8_t *data, uint32_t length);

#endif // !__SRAM_H__