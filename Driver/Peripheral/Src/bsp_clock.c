#include "bsp_clock.h"

/**
 * @brief 时钟设置函数
 * 
 * @param pllm: 主PLL和音频PLL预分频系数（进PLL之前的分频）, 取值范围：2 ~ 63
 * @param plln: 主PLL倍频系数（PLL倍频）， 取值范围: 64 ~ 432
 * @param pllp: 主PLL的p分频系数（PLL之后的分频），分频后作为系统时钟，取值范围: [2, 4, 6, 8]（仅限这4个值）
 * @param pllq: 主PLL的q分频系数（PLL之后的分频），取值范围: 2 ~ 15
 * @return uint8_t 错误代码: 0，成功；其它值，错误；
 * 
 * @note 
 *      外部晶振为 8M的时候, 推荐值:  pllm = 8, plln = 336, pllp = 2, pllq = 7.
 *      F(vco): F(hse) / pllm * plln = 8 / 8 * 336 = 336Mhz
 *      F(PLL48CK) = F(vco) / pllq = F(hse) / pllm * plln / pllq = 336 / 7 = 48Mhz
 *      F(SYSCLK) = F(PLLCLK) = F(vco) / pllp = F(hse) / pllm * plln / pllp = 336 / 2 = 168Mhz
 *      F(AHB1/2/3) = F(SYSCLK) = 168Mhz
 *      F(HCLK) = F(AHB1/2/3) = 168Mhz
 *      F(APB1) = pll_p_ck / 4 = 42Mhz
 *      F(APB2) = pll_p_ck / 2 = 84Mhz
 */
uint8_t System_Clock_Init(uint32_t pllm, uint32_t plln, uint32_t pllp, uint32_t pllq)
{
    HAL_StatusTypeDef result = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();                                                 // 使能PWR时钟

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);              // 设置调压器输出电压级别，以便在器件未以最大频率工作

    // 使能HSE，并选择HSE作为PLL时钟源，配置PLL1，开启USB时钟
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;                  // 时钟源为HSE
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;                                    // 打开HSE

    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                                // 打开PLL
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;                        // PLL时钟源选择HSE

    RCC_OscInitStruct.PLL.PLLM = pllm;
    RCC_OscInitStruct.PLL.PLLN = plln;
    RCC_OscInitStruct.PLL.PLLP = pllp;
    RCC_OscInitStruct.PLL.PLLQ = pllq;

    result = HAL_RCC_OscConfig(&RCC_OscInitStruct);                             // 初始化RCC
    if(result != HAL_OK)
    {
        // 时钟初始化失败，可以在这里加入自己的处理
        return 1;
    }

    // 选中PLL作为系统时钟源并且配置HCLK,PCLK1和PCLK2
    RCC_ClkInitStruct.ClockType = ( RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;                   // 设置系统时钟时钟源为PLL
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                          // AHB分频系数为1
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;                           // APB1分频系数为4
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;                           // APB2分频系数为2

    result = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);          // 同时设置FLASH延时周期为5WS，也就是6个CPU周期
    if(result != HAL_OK)
    {
        // 时钟初始化失败，可以在这里加入自己的处理
        return 2;
    }
  
    // STM32F405x/407x/415x/417x Z版本的器件支持预取功能
    if (HAL_GetREVID() == 0x1001)
    {
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();                                   // 使能flash预取
    }

    SystemCoreClockUpdate();                                                    // 更新系统时钟

    return 0;
}