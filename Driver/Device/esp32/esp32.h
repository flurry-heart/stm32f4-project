#ifndef __ESP32_H__
#define __ESP32_H__

#include <stdbool.h>

#include "stm32f4xx_hal.h"

#include "bsp_uart.h"

extern UART_HandleTypeDef *pg_uart_esp32_handler;
extern UART_FrameData_t *pg_uart_esp32_frameData;

void ESP32_Init(UART_HandleTypeDef *huart, UART_FrameData_t *frameData);
bool ESP32_SendAtCmd(char *cmd, char *ack, uint32_t timeOut);
void ESP32_EchoConfig(uint8_t echo);
void ESP32_ExitUnvarnished(void);

#endif // !__ESP32_H__