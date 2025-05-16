#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f4xx_hal.h"

#define WKUP_GPIO_PORT          GPIOA
#define WKUP_GPIO_PIN           GPIO_PIN_0
#define WKUP_GPIO_CLK_ENABLE()  do{ \
                                    __HAL_RCC_GPIOA_CLK_ENABLE(); \
                                } while(0);

#define KEY1_GPIO_PORT          GPIOE
#define KEY1_GPIO_PIN           GPIO_PIN_4
#define KEY1_GPIO_CLK_ENABLE()  do{ \
                                    __HAL_RCC_GPIOE_CLK_ENABLE(); \
                                } while(0);

#define KEY2_GPIO_PORT          GPIOE
#define KEY2_GPIO_PIN           GPIO_PIN_3
#define KEY2_GPIO_CLK_ENABLE()  do{ \
                                    __HAL_RCC_GPIOE_CLK_ENABLE(); \
                                } while(0);

#define KEY3_GPIO_PORT          GPIOE
#define KEY3_GPIO_PIN           GPIO_PIN_2
#define KEY3_GPIO_CLK_ENABLE()  do{ \
                                    __HAL_RCC_GPIOE_CLK_ENABLE(); \
                                } while(0);

/* 读取按键对应的GPIO引脚的电平状态 */
#define WK_UP                   HAL_GPIO_ReadPin(WKUP_GPIO_PORT, WKUP_GPIO_PIN)
#define KEY1                    HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN)
#define KEY2                    HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN)
#define KEY3                    HAL_GPIO_ReadPin(KEY3_GPIO_PORT, KEY3_GPIO_PIN)


/* 对应按键按下时代表的数值 */
#define WKUP_PRESS              1
#define KEY1_PRESS              2
#define KEY2_PRESS              3
#define KEY3_PRESS              4

void Key_Init(void);
uint8_t Key_Scan(uint8_t mode);

#endif // !__KEY_H__