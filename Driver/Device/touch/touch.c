#include "touch.h"

/**
 * @brief 触摸屏初始化
 * 
 */
void Touch_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    RCC_TOUCH_CS_GPIO_CLK_ENABLE();
    RCC_TOUCH_PEN_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = TOUCH_CS_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TOUCH_CS_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = TOUCH_PEN_GPIO_PIN;
    HAL_GPIO_Init(TOUCH_PEN_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief 触摸屏读取AD值
 * 
 * @param command 要写入的命令
 * @return uint16_t 读取的数据
 */
uint16_t Touch_ReadAD(uint8_t command)
{
    uint16_t data = 0;

    TOUCH_CS(0);                                                                // 选中设备

    // MCU向XPT2046发送数据
    BSP_Simulate_SPI_SwapOneByte(command);

    // 过滤忙信号
    SPI_SCK(0);
    Delay_us(1);
    SPI_SCK(1);
    Delay_us(1);

    // MCU读取XTP2046返回数据
    data = BSP_Simulate_SPI_SwapOneByte(0x00);
    data <<= 8;
    data |= BSP_Simulate_SPI_SwapOneByte(0x00);

    TOUCH_CS(1);                                                                // 释放设备

    return (data >> 4);
}