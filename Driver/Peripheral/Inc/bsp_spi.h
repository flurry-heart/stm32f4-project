#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#include "stm32f4xx_hal.h"

#include "bsp_systick.h"

#define SPI_SCK_GPIO_PORT               GPIOB
#define SPI_SCK_GPIO_PIN                GPIO_PIN_0
#define RCC_SPI_SCK_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPI_MISO_GPIO_PORT              GPIOB
#define SPI_MISO_GPIO_PIN               GPIO_PIN_2
#define RCC_SPI_MISO_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPI_MOSI_GPIO_PORT              GPIOF
#define SPI_MOSI_GPIO_PIN               GPIO_PIN_11
#define RCC_SPI_MOSI_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()

#define SPI_SCK(x)                      do{ x ? \
                                            HAL_GPIO_WritePin(SPI_SCK_GPIO_PORT,SPI_SCK_GPIO_PIN, GPIO_PIN_SET):\
                                            HAL_GPIO_WritePin(SPI_SCK_GPIO_PORT,SPI_SCK_GPIO_PIN, GPIO_PIN_RESET);\
                                        }while(0)

#define SPI_MISO()                      HAL_GPIO_ReadPin(SPI_MISO_GPIO_PORT, SPI_MISO_GPIO_PIN)

#define SPI_MOSI(x)                     do{ x ? \
                                            HAL_GPIO_WritePin(SPI_MOSI_GPIO_PORT, SPI_MOSI_GPIO_PIN, GPIO_PIN_SET):\
                                            HAL_GPIO_WritePin(SPI_MOSI_GPIO_PORT, SPI_MOSI_GPIO_PIN, GPIO_PIN_RESET);\
                                        }while(0)

extern SPI_HandleTypeDef g_spi1_handle;

void BSP_SPI_Init(SPI_HandleTypeDef *hspi, SPI_TypeDef *SPIx,uint32_t CLKPolarity, uint32_t CLKPhase, uint32_t perscaler,uint32_t firstBit);
uint8_t BSP_SPI_SwapOneByte(SPI_HandleTypeDef *hspi, uint8_t data);
void BSP_SPI_SendBytes(SPI_HandleTypeDef *hspi, uint8_t *data, uint16_t length);

void BSP_Simulate_SPI_Init(void);
uint8_t BSP_Simulate_SPI_SwapOneByte(uint8_t data);

#endif // !__BSP_SPI_H__