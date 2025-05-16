#include "bsp_rng.h"

RNG_HandleTypeDef g_rng_handle;

/**
 * @brief RNG初始化函数
 * 
 * @return uint8_t 0: 初始化成功; 1: 初始化失败
 */
uint8_t BSP_RNG_Init(void)
{
    uint16_t time = 0;

    g_rng_handle.Instance = RNG;
    HAL_RNG_Init(&g_rng_handle);

    // 等待RNG初始化完成
    while ((!__HAL_RNG_GET_FLAG(&g_rng_handle, RNG_FLAG_DRDY) == RESET) && (time < 1000))
    {
        time++;
        HAL_Delay(1);
    }
    
    if (time >= 1000)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief RNG底层初始化函数
 * 
 * @param hrng 
 */
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
    if (hrng->Instance == RNG)
    {
        __HAL_RCC_RNG_CLK_ENABLE();
    }
}

/**
 * @brief 生成一个随机数
 * 
 * @return uint32_t 32位的随机数值
 */
int BSP_RNG_GetRandomNumber(void)
{
    uint32_t random_number = 0;
    HAL_RNG_GenerateRandomNumber(&g_rng_handle, &random_number);

    return random_number;
}

/**
 * @brief 获取某一个范围内的随机数
 * 
 * @param min 最小值
 * @param max 最大值
 * @return uint32_t 32位的随机数值
 */
int BSP_RNG_GetRandomNumberInRange(int min, int max)
{
    uint32_t random_number = 0;
    HAL_RNG_GenerateRandomNumber(&g_rng_handle, &random_number);

    return random_number % (max - min + 1) + min;
}