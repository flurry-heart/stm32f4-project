#include "bsp_rs485.h"

UART_HandleTypeDef *pg_rx485_uart_handle;
UART_FrameData_t *pg_rs485_uart_frame_data;

/**
 * @brief RX485初始化函数
 * 
 * @param huart 串口句柄
 * @param pFrameDataa 串口接收数据帧
 */
void BSP_RS485_Init(UART_HandleTypeDef *huart, UART_FrameData_t *pFrameDataa)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    pg_rx485_uart_handle = huart;
    pg_rs485_uart_frame_data = pFrameDataa;

    RS485_RE_GPIO_CLK_ENABLE();                                                 // 使能RS485 RE引脚时钟

    GPIO_InitStruct.Pin = RS485_RE_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RS485_RE_GPIO_PORT, &GPIO_InitStruct);

    RS485_RE(0);                                                                // 进入接收模式
}

/**
 * @brief RS485发送数据函数
 * 
 * @param buffer 要发送数据缓冲区的指针
 * @param length 要发送数据的大小
 */
void BSP_RS485_SendData(uint8_t *data, uint16_t length)
{
    RS485_RE(1);                                                                // 进入发送模式
    HAL_UART_Transmit(pg_rx485_uart_handle, data, length, 0xFFFF);              // UART发送数据
    RS485_RE(0);                                                                // 进入接收模式
}