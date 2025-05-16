#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "stm32f4xx_hal.h"

#include "bsp_spi.h"
#include "bsp_systick.h"

#include "lcd/lcd.h"

#define TOUCH_CS_GPIO_PORT                  GPIOC
#define TOUCH_CS_GPIO_PIN                   GPIO_PIN_3
#define RCC_TOUCH_CS_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

#define TOUCH_PEN_GPIO_PORT                 GPIOB
#define TOUCH_PEN_GPIO_PIN                  GPIO_PIN_1
#define RCC_TOUCH_PEN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define TOUCH_CS(x)                         do{ x ? \
                                                HAL_GPIO_WritePin(TOUCH_CS_GPIO_PORT, TOUCH_CS_GPIO_PIN, GPIO_PIN_SET):\
                                                HAL_GPIO_WritePin(TOUCH_CS_GPIO_PORT, TOUCH_CS_GPIO_PIN, GPIO_PIN_RESET);\
                                            }while(0)


#define TOUCH_PEN(x)                        do{ x ? \
                                                HAL_GPIO_WritePin(TOUCH_PEN_GPIO_PORT, TOUCH_PEN_GPIO_PIN, GPIO_PIN_SET):\
                                                HAL_GPIO_WritePin(TOUCH_PEN_GPIO_PORT, TOUCH_PEN_GPIO_PIN, GPIO_PIN_RESET);\
                                            }while(0)

void Touch_Init(void);
uint16_t Touch_ReadAD(uint8_t command);

#endif // !__TOUCH_H__