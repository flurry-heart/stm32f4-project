#include "bsp_iwdg.h"

IWDG_HandleTypeDef g_iwdg_handle;                                               // 独立看门狗句柄

/**
 * @brief IWDG初始化函数
 * 
 * @param prescaler 预分频值
 * @param reload 重载值
 */
void BSP_IWDG_Init(uint8_t prescaler, uint16_t reload)
{
    g_iwdg_handle.Instance = IWDG;                                              // IWDG寄存器基地址
    g_iwdg_handle.Init.Prescaler = prescaler;                                   // 预分频值
    g_iwdg_handle.Init.Reload = reload;                                         // 重载值
    HAL_IWDG_Init(&g_iwdg_handle);
}