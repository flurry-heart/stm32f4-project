#include "led.h"

/**
 * @brief led初始化函数
 * 
 */
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOF_CLK_ENABLE();                                               // 使能GPIOF的时钟

    GPIO_InitStruct.Pin = GPIO_PIN_9;                                           // GPIO引脚
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;                                 // 推挽输出模式
    GPIO_InitStruct.Pull = GPIO_NOPULL;                                         // 不使用上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                               // 高速模式
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);                                     // GPIO初始化

    GPIO_InitStruct.Pin = GPIO_PIN_10;                                          // GPIO引脚
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);                                     // GPIO初始化

    LED_SetStatus(GPIOF, GPIO_PIN_9, LED_OFF);
    LED_SetStatus(GPIOF, GPIO_PIN_10, LED_OFF);
}

/**
 * @brief 设置LED状态函数
 * 
 * @param LED_Port LED的GPIO端口
 * @param LED_Pin LED的GPIO引脚
 * @param status LED的状态枚举值
 */
void LED_SetStatus(GPIO_TypeDef *LED_Port, uint16_t LED_Pin, LED_State status)
{
    if(status == LED_ON)
    {
        HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_SET);
    }
}