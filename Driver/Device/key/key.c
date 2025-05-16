#include "key.h"

/**
 * @brief 按键初始化函数
 * 
 */
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    WKUP_GPIO_CLK_ENABLE();
    KEY1_GPIO_CLK_ENABLE();
    KEY2_GPIO_CLK_ENABLE();
    KEY3_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = WKUP_GPIO_PIN;                                        // GPIO引脚
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;                                     // 输入模式
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;                                       // 使用下拉
    HAL_GPIO_Init(WKUP_GPIO_PORT, &GPIO_InitStruct);                            // GPIO初始化

    GPIO_InitStruct.Pin = KEY1_GPIO_PIN;                                        // GPIO引脚
    GPIO_InitStruct.Pull = GPIO_PULLUP;                                         // 使用上拉
    HAL_GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStruct);                            // GPIO初始化

    GPIO_InitStruct.Pin = KEY2_GPIO_PIN;                                        // GPIO引脚
    HAL_GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStruct);                            // GPIO初始化

    GPIO_InitStruct.Pin = KEY3_GPIO_PIN;                                        // GPIO引脚
    HAL_GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStruct);                            // GPIO初始化
}

/**
 * @brief 按键扫描函数
 * 
 * @param mode 0：不支持连续按；1：支持连续按
 * @return uint8_t WKUP_PRES，1，WKUP按下
 *                 KEY1_PRES，2，KEY0按下
 *                 KEY2_PRES，3，KEY1按下
 *                 KEY3_PRES，4，KEY2按下
 * 
 * @note 该函数有响应优先级，同时按下多个按键：WK_UP > KEY3 > KEY2 > KEY1
 */
uint8_t Key_Scan(uint8_t mode)
{
    static uint8_t flag = 1;                                                    // 按键按松开标志
    uint8_t keyValue = 0;

    flag = (mode ? 1 : flag);                                                   // 支持连按

    if (flag && ( WK_UP == 1 || KEY1 == 0 || KEY2 == 0 || KEY3 == 0))           // 按键松开标志为1, 且有任意一个按键按下了
    {
        HAL_Delay(10);                                                          // 按键消抖
        flag = 0;                 
        // 再次读取GPIO引脚的电平
        keyValue = ((KEY1 == 0) ? KEY1_PRESS : keyValue);
        keyValue = ((KEY2 == 0) ? KEY2_PRESS : keyValue);
        keyValue = ((KEY3 == 0) ? KEY3_PRESS : keyValue);
        keyValue = ((WK_UP == 1) ? WKUP_PRESS : keyValue);                
    }
    else if (WK_UP == 0 && KEY1 == 1 && KEY2 == 1 && KEY3 == 1)                 // 没有任何按键按下, 标记按键松开
    {
        flag = 1;
    }

    return keyValue;                                                            // 按键没有按下返回0 
}