#include "tpad.h"

TIM_HandleTypeDef *pg_tpad_timer_handle;

uint16_t g_tpad_default_value ;

static void TPad_Reset(void);
static uint16_t TPad_GetValue(void);
static uint16_t TPad_GetMaxValue(uint8_t n);

/**
 * @brief 电容触摸按键初始化函数
 * 
 */
void TPad_Init(TIM_HandleTypeDef *htim)
{
    int buff[10] = {0};
    uint8_t temp = 0;

    pg_tpad_timer_handle = htim;

    // 循环取10次值
    for (uint8_t i = 0; i < 10; i++)
    {
        buff[i] = TPad_GetValue();
        HAL_Delay(10);
    }

    // 冒泡排序
    BubbleSort(buff, sizeof(buff)/sizeof(buff[0]));

    // 对中间6个值取平均
    for (uint8_t i = 2; i < 8; i++)
    {
        temp += buff[i];
    }

    g_tpad_default_value = temp / 6;
}

/**
 * @brief 电容触摸按键复位函数
 * 
 */
static void TPad_Reset(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 推挽输出，电容放电
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

    HAL_Delay(5);

    pg_tpad_timer_handle->Instance->SR = 0;                                     // 清除标记
    pg_tpad_timer_handle->Instance->CNT = 0;                                    // 清空计数器

    // 复用功能
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
 * @brief 获取定时器捕获值函数
 * 
 * @return uint16_t 定时器的捕获值
 */
static uint16_t TPad_GetValue(void)
{
    TPad_Reset();

    while (__HAL_TIM_GET_FLAG(pg_tpad_timer_handle, TIM_FLAG_CC1) == RESET)     // 检测通道捕获到上升沿
    {
        // 超时了直接返回计数器寄存器的值
        if (pg_tpad_timer_handle->Instance->CNT > TPAD_MAX_VALUE  - 500)
        {
            return pg_tpad_timer_handle->Instance->CNT;
        }
    }
    // 返回捕获/比较寄存器的值
    return pg_tpad_timer_handle->Instance->CCR1;
}

/**
 * @brief 获取最大值函数
 * 
 * @param n 数据个数
 * @return uint16_t 最大的值
 */
static uint16_t TPad_GetMaxValue(uint8_t n)
{
    uint16_t temp = 0;
    uint16_t maxValue = 0;

    while (n--)
    {
        temp = TPad_GetValue();
        maxValue = (temp > maxValue) ? temp : maxValue;
    }
    return maxValue;
}

/**
 * @brief 电容触摸按键扫描函数
 * 
 * @param mode 0：不支持连续按；1：支持连续按
 * @return uint8_t 0：未按下；1：按下
 */
uint8_t TPad_Scan(uint8_t mode)
{
    static uint8_t keyen = 0;
    uint8_t result = 0;
    uint8_t sample = 3;                                                         // 默认采样次数为3
    uint8_t value = 0;

    if (mode)
    {
        sample = 6;                                                             // 支持连按的时候，设置采样次数为6次
        keyen = 0;                                                              // 支持连按，每次调用该函数都可以检测
    }

    value = TPad_GetMaxValue(sample);
  
    if (value > (g_tpad_default_value + TPAD_GATE_VALUE))                       // 有效按下
    {
        result = (keyen == 0) ? 1 : result;                                     // keyen==0时表示有按键按下
        keyen = 3;
    }
    keyen ? --keyen : keyen;
  
    return result;
}