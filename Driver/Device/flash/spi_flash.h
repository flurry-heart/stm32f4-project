#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f4xx_hal.h"

#include "bsp_spi.h"
#include "spi_flash_register.h"

#define W25Q16                              16
#define W25Q32                              32
#define W25Q64                              64
#define W25Q128                             128
#define W25Q256                             256

#define W25Qxxx                             W25Q128

#define SPI_FLASH_CS_GPIO_PORT              GPIOB
#define SPI_FLASH_CS_GPIO_PIN               GPIO_PIN_14
#define RCC_SPI_FLASH_CS_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPI_FLASH_CS(x)                     do{ x ? \
                                                HAL_GPIO_WritePin(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_GPIO_PIN, GPIO_PIN_SET):\
                                                HAL_GPIO_WritePin(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_GPIO_PIN, GPIO_PIN_RESET);\
                                            }while(0)

void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(uint32_t address);
void SPI_FLASH_ReadData(uint32_t address, uint8_t *data, uint16_t length);
void SPI_FLASH_WriteData(uint32_t address, uint8_t *data, uint16_t length);
uint32_t SPI_FLASH_ReadID(void);

#endif // !__FLASH_H__