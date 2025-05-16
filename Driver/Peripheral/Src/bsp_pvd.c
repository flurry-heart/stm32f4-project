#include "bsp_pvd.h"

/**
 * @brief 初始化PVD电压监视器函数
 * 
 * @param level 电压等级，可选值: PWR_PVDLEVEL_x, x 可选范围：[0, 7]，对应电压范围: 2.2V ~ 2.9V
 */
void BSP_PVD_Init(uint32_t level)
{
    PWR_PVDTypeDef PWR_PVDInit = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    
    PWR_PVDInit.PVDLevel = level;                                               // 电压监测等级
    PWR_PVDInit.Mode = PWR_PVD_MODE_IT_RISING_FALLING;                          // 使用中断线的上升沿和下降沿双边缘触发
    HAL_PWR_ConfigPVD(&PWR_PVDInit);

    HAL_PWR_EnablePVD();                                                        // 使能PVD

    // 设置PVD中断优先级
    HAL_NVIC_SetPriority(PVD_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(PVD_IRQn);
}

/**
 * @brief PVD中断服务函数
 * 
 */
void PVD_IRQHandler(void)
{
    HAL_PWR_PVD_IRQHandler();
}

/**
 * @brief PVD中断回调函数
 * 
 */
void HAL_PWR_PVDCallback(void)
{
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO))                                      // 电压比PLS设置的还低
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
    }
}
