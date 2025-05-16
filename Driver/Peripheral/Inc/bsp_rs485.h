#ifndef __BSP_RS485_H__
#define __BSP_RS485_H__

#include "stm32f4xx_hal.h"

#include "bsp_uart.h"

extern UART_HandleTypeDef *pg_rx485_uart_handle;
extern UART_FrameData_t *pg_rs485_uart_frame_data;

#define RS485_RE_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOG_CLK_ENABLE()
#define RS485_RE_GPIO_PORT          GPIOG
#define RS485_RE_GPIO_PIN           GPIO_PIN_8
#define RS485_RE(x)                 do { x ? \
                                        HAL_GPIO_WritePin(RS485_RE_GPIO_PORT, RS485_RE_GPIO_PIN, GPIO_PIN_SET) : \
                                        HAL_GPIO_WritePin(RS485_RE_GPIO_PORT, RS485_RE_GPIO_PIN, GPIO_PIN_RESET); \
                                    } while (0);

void BSP_RS485_Init(UART_HandleTypeDef *huart, UART_FrameData_t *pFrameDataa);
void BSP_RS485_SendData(uint8_t *buffer, uint16_t length);

#endif // !__BSP_RS485_H__