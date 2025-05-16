#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "stm32f4xx_hal.h"
#include "bsp_i2c.h"

#define AT24C01             128
#define AT24C02             256
#define AT24C04             512
#define AT24C08             1024
#define AT24C16             2048
#define AT24C32             4096
#define AT24C64             8192
#define AT24C128            16384
#define AT24C256            32768
#define AT24C512            65536

#define EEPROM_TYPE         AT24C02
#define EEPROM_ADDRESS      0x50

uint8_t EEPROM_ReadOneByte(uint16_t address);
void EEPROM_ReadBytes(uint16_t address, uint8_t *pBuffer, uint16_t length);
void EEPROM_WriteOneByte(uint16_t address, uint8_t data);
void EEPROM_WriteBytes(uint16_t address, uint8_t *pBuffer, uint16_t length);

#endif // !__EEPROM_H__