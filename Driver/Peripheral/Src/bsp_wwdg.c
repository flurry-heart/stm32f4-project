#include "bsp_wwdg.h"

WWDG_HandleTypeDef g_wwdg_handle;

/**
 * @brief WWDG初始化函数
 * 
 * @param prescaler 预分频值
 * @param count 计数值
 * @param window 窗口值
 */
void BSP_WWDG_Init(uint16_t prescaler, uint8_t count, uint8_t window)
{
    g_wwdg_handle.Instance = WWDG;                                              // WWDG寄存器地址
    g_wwdg_handle.Init.Prescaler = prescaler;                                   // 预分频值
    g_wwdg_handle.Init.Counter = count;                                         // 计数值
    g_wwdg_handle.Init.Window = window;                                         // 窗口值 
    g_wwdg_handle.Init.EWIMode = WWDG_EWI_ENABLE;                               // 提前唤醒中断使能
    HAL_WWDG_Init(&g_wwdg_handle);
}

/**
 * @brief WWDG底层初始化函数
 * 
 * @param hwwdg WWDG句柄
 */
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
    if (hwwdg->Instance == WWDG)
    {
        __HAL_RCC_WWDG_CLK_ENABLE();                                            // 使能WWDG时钟

        HAL_NVIC_SetPriority(WWDG_IRQn, 2, 0);                                  // 设置WWDG中断优先级
        HAL_NVIC_EnableIRQ(WWDG_IRQn);                                          // 使能WWDG中断
    }
}

/**
 * @brief WWDG中断服务函数
 * 
 */
void WWDG_IRQHandler(void)
{
    if (__HAL_WWDG_GET_FLAG(&g_wwdg_handle, WWDG_FLAG_EWIF) != RESET)           // 检测WWDG提前唤醒中断标志位
    {
        __HAL_WWDG_CLEAR_FLAG(&g_wwdg_handle, WWDG_FLAG_EWIF);                  // 清除WWDG提前唤醒中断标志位

        HAL_WWDG_Refresh(&g_wwdg_handle);                                       // 喂狗
    }
}