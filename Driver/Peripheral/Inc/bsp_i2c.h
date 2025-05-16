#ifndef __BSP_I2C_H__
#define __BSP_I2C_H__

#include "stm32f4xx_hal.h"

#include "bsp_systick.h"

#define I2C_SCL_GPIO_PORT           GPIOB
#define I2C_SCL_GPIO_PIN            GPIO_PIN_8
#define I2C_SCL_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2C_SDA_GPIO_PORT           GPIOB
#define I2C_SDA_GPIO_PIN            GPIO_PIN_9
#define I2C_SDA_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2C_SCL(x)                  do{ x ? \
                                        HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT,I2C_SCL_GPIO_PIN, GPIO_PIN_SET):\
                                        HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT,I2C_SCL_GPIO_PIN, GPIO_PIN_RESET);\
                                    }while(0)

#define I2C_SDA(x)                  do{ x ? \
                                        HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN, GPIO_PIN_SET):\
                                        HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN, GPIO_PIN_RESET);\
                                    }while(0)

#define I2C_READ_SDA()              HAL_GPIO_ReadPin(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN)

#define I2C_DELAY()                 Delay_us(10)

extern I2C_HandleTypeDef g_i2c1_handle;

void BSP_I2C_Init(I2C_HandleTypeDef *hi2c, I2C_TypeDef *I2Cx, uint32_t speed);

void BSP_Simulate_I2C_Init(void);
void BSP_Simulate_I2C_Start(void);
void BSP_Simulate_I2C_Stop(void);
uint8_t BSP_Simulate_I2C_WaitAck(void);
void BSP_Simulate_I2C_SendAck(uint8_t ack);
uint8_t BSP_Simulate_I2C_ReadOneByte(uint8_t ack);
void BSP_Simulate_I2C_SendOneByte(uint8_t data);

#endif // !__BSP_I2C_H__