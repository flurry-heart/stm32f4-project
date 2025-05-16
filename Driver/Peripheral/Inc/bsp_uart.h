#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

#define UART_RECEIVE_LENGTH 200

typedef struct UART_FrameData_t
{
    uint16_t length;                                                            // 数据长度
    bool finsh;                                                                 // 是否接收完成
    uint8_t data[UART_RECEIVE_LENGTH];                                          // 帧接收缓冲
} UART_FrameData_t;

extern UART_HandleTypeDef g_usart1_handle;                                      // USART1句柄
extern UART_HandleTypeDef g_usart2_handle;                                      // USART1句柄

extern UART_FrameData_t g_usart1_frame_data;                                    // USART1帧数据
extern UART_FrameData_t g_usart2_frame_data;                                    // USART2帧数据

void BSP_UART_Init(UART_HandleTypeDef *huart, USART_TypeDef *UARTx, uint32_t band);
uint16_t BSP_UART_GetFrameDataLength(UART_FrameData_t *frameData);
void BSP_UART_ClearFrameData(UART_FrameData_t *frameData);
void BSP_UART_Printf(UART_HandleTypeDef *huart, char *fmt, ...);


#endif // !__BSP_UART_H__