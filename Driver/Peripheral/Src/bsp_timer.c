#include "bsp_timer.h"

TIM_HandleTypeDef g_timer1_handle;
TIM_HandleTypeDef g_timer2_handle;
TIM_HandleTypeDef g_timer5_handle;
TIM_HandleTypeDef g_timer6_handle;
TIM_HandleTypeDef g_timer8_handle;
TIM_HandleTypeDef g_timer11_handle;
TIM_HandleTypeDef g_timer13_handle;
TIM_HandleTypeDef g_timer14_handle;

uint8_t g_timer5_channel1_capture_status;                                       // 定时器5通道1的输入捕获状态
uint16_t g_timer5_channel1_capture_value;                                       // 定时器5通道1的输入捕获值
uint16_t g_timer5_channel1_update_count;                                        // 定时器5通道1的更新计数值

uint8_t g_timer5_channel1_pwm_in_status;                                       // 0: 未检测到PWM信号; 1: 检测到PWM信号
uint16_t g_timer5_channel1_pwm_in_period;                                       // PWM周期
uint16_t g_timer5_channel1_pwm_in_high_level_duration;                           // 高电平持续时间

uint32_t g_timer8_channel1_npwm_remain;

uint32_t g_timer_1ms_ticks;

/**
 * @brief 定时器定时功能初始化函数
 * 
 * @param htim 定时器句柄
 * @param TIMx 定时器寄存器基地址，可选值: TIMx, x可选范围: 1 ~ 14
 * @param prescaler 预分频系数，可选值: 0 ~ 65535
 * @param period 自动重装载值，可选值: 0 ~ 65535
 * 
 * @note 默认为向上计数模式
 */
void BSP_Timer_Base_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period)
{
    htim->Instance = TIMx;                                                      // 定时器寄存器基地址
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;                                // 计数模式
    htim->Init.Prescaler = prescaler;                                           // 预分频系数
    htim->Init.Period = period;                                                 // 自动重装载值
    HAL_TIM_Base_Init(htim);
}

/**
 * @brief 基本定时器底层初始化函数
 * 
 * @param htim 定时器句柄
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        __HAL_RCC_TIM6_CLK_ENABLE();                                            // 使能定时器6的时钟

        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);                                      // 使能定时器6中断
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 4, 0);                              // 设置中断优先级
    }
}

/**
 * @brief 定时器PWM功能初始化函数
 * 
 * @param htim 定时器句柄
 * @param TIMx 定时器寄存器基地址，可选值: TIMx, x可选范围: 1 ~ 5, 8 ~ 14
 * @param prescaler 预分频系数，可选值: 0 ~ 65535
 * @param period 自动重装载值，可选值: 0 ~ 65535
 * @param channel 输出PWM的通道，可选值: TIM_CHANNEL_x, x可选范围: 1 ~ 4
 * @param polarity 输出比较极性，可选值: [TIM_OCPOLARITY_LOW, TIM_OCPOLARITY_HIGH]
 * @param pluse 输出比较值，可选值: 0 ~ 65535
 */
void BSP_Timer_PWM_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period, uint32_t channel, uint32_t polarity, uint32_t pluse)
{
    TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

    htim->Instance = TIMx;                                                      // 定时器寄存器基地址
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;                                // 计数模式
    htim->Init.Prescaler = prescaler;                                           // 预分频系数
    htim->Init.Period = period;                                                 // 自动重装载值
    HAL_TIM_PWM_Init(htim);

    TIM_OC_InitStruct.OCMode = TIM_OCMODE_PWM1;                                 // PWM模式1
    TIM_OC_InitStruct.Pulse = pluse;                                            // 比较值
    TIM_OC_InitStruct.OCPolarity = polarity;                                    // 输出比较极性
    HAL_TIM_PWM_ConfigChannel(htim, &TIM_OC_InitStruct, channel);
}

/**
 * @brief 定时器PWM互补输出功能初始化函数
 * 
 * @param htim 定时器句柄
 * @param TIMx 定时器寄存器基地址，可选值: TIMx, x可选范围: [1, 8]
 * @param prescaler 预分频系数，可选值: 0 ~ 65535
 * @param period 自动重装载值，可选值: 0 ~ 65535
 * @param channel 输出PWM的通道，可选值: TIM_CHANNEL_x, x可选范围: 1 ~ 4
 * @param polarity 输出比较极性，可选值: [TIM_OCPOLARITY_LOW, TIM_OCPOLARITY_HIGH]
 * @param NPolarity 输出比较极性，可选值: [TIM_OCNPOLARITY_LOW, TIM_OCNPOLARITY_HIGH]
 * @param pluse 输出比较值，可选值: 0 ~ 65535
 * @param deadTime 死区时间，可选值: 0 ~ 65535
 * @param breakPolarity 刹车极性，可选值: [TIM_BREAKPOLARITY_LOW, TIM_BREAKPOLARITY_HIGH]
 */
void BSP_Timer_ComplementaryOutput_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period, uint32_t channel, uint32_t polarity, uint32_t NPolarity, uint32_t pluse, uint32_t deadTime, uint32_t breakPolarity)
{
    TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_BreakDeadTimeConfigTypeDef TIM_BreakDeadTimeConfigStruct = {0};

    htim->Instance = TIMx;                                                      // 定时器寄存器基地址
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;                                // 计数模式
    htim->Init.Prescaler = prescaler;                                           // 预分频系数
    htim->Init.Period = period;                                                 // 自动重装载值
    HAL_TIM_PWM_Init(htim);

    TIM_OC_InitStruct.OCMode = TIM_OCMODE_PWM1;                                 // PWM模式1
    TIM_OC_InitStruct.Pulse = pluse;                                            // 比较值
    TIM_OC_InitStruct.OCPolarity = polarity;                                    // 输出比较极性
    TIM_OC_InitStruct.OCIdleState = TIM_OCIDLESTATE_RESET;                      // 输出比较空闲状态
    TIM_OC_InitStruct.OCNPolarity = NPolarity;                                  // 互补输出比较极性
    TIM_OC_InitStruct.OCNIdleState = TIM_OCNIDLESTATE_RESET;                    // 互补输出比较空闲状态
    HAL_TIM_PWM_ConfigChannel(htim, &TIM_OC_InitStruct, channel);


    TIM_BreakDeadTimeConfigStruct.OffStateRunMode = TIM_OSSR_DISABLE;           // 运行模式下的关闭状态选择
    TIM_BreakDeadTimeConfigStruct.OffStateIDLEMode = TIM_OSSI_DISABLE;          // 空闲模式下的关闭状态选择
    TIM_BreakDeadTimeConfigStruct.LockLevel = TIM_LOCKLEVEL_OFF;                // 寄存器锁定设置
    TIM_BreakDeadTimeConfigStruct.DeadTime = deadTime;                          // 死区时间
    TIM_BreakDeadTimeConfigStruct.BreakState = TIM_BREAK_ENABLE;                // 刹车使能
    TIM_BreakDeadTimeConfigStruct.BreakPolarity = breakPolarity;                // 刹车极性
    TIM_BreakDeadTimeConfigStruct.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE; // 自动恢复输出使能
    HAL_TIMEx_ConfigBreakDeadTime(htim, &TIM_BreakDeadTimeConfigStruct);
}

/**
 * @brief 定时器PWM模式底层初始化函数
 * 
 * @param htim 定时器句柄
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (htim->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE();                                            // 使能TIM1的时钟
        __HAL_RCC_GPIOA_CLK_ENABLE();                                           // 使能TIM1的Channel 1和Channel N1对应的GPIO时钟

        GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;                          // TIM1的Channel 1和Channel N1对应的GPIO引脚
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                                 // 复用功能
        GPIO_InitStruct.Pull = GPIO_NOPULL;                                     // 不使用上下拉
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                           // GPIO输出速度
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;                              // 复用功能选择
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (htim->Instance == TIM8)
    {
        __HAL_RCC_TIM8_CLK_ENABLE();                                            // 使能TIM8的时钟
        __HAL_RCC_GPIOC_CLK_ENABLE();                                           // 使能TIM8的Channel 1对应的GPIO时钟

        GPIO_InitStruct.Pin = GPIO_PIN_6;                                       // TIM8的Channel 1
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                                 // 复用功能
        GPIO_InitStruct.Pull = GPIO_NOPULL;                                     // 不使用上下拉
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                           // GPIO输出速度
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;                              // 复用功能选择
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
    }
    else if (htim->Instance == TIM14)
    {
        __HAL_RCC_TIM14_CLK_ENABLE();                                           // 使能TIM14的时钟
        __HAL_RCC_GPIOF_CLK_ENABLE();                                           // 使能TIM14的Channel 1对应的GPIO时钟

        GPIO_InitStruct.Pin = GPIO_PIN_9;                                       // TIM14的Channel 1对应的GPIO引脚
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                                 // 复用功能
        GPIO_InitStruct.Pull = GPIO_NOPULL;                                     // 不使用上下拉
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                           // GPIO输出速度
        GPIO_InitStruct.Alternate = GPIO_AF9_TIM14;                             // 复用功能选择
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    }
}

/**
 * @brief 定时器输入捕获初始化函数
 * 
 * @param htim 定时器句柄
 * @param TIMx 定时器寄存器基地址，可选值: TIMx, x可选范围: 1 ~ 5, 8 ~ 14
 * @param prescaler 预分频系数，可选值: 0 ~ 65535
 * @param period 自动重装载值，可选值: 0 ~ 65535
 * @param channel 输入捕获的通道，可选值: TIM_CHANNEL_x, x可选范围: 1 ~ 4
 * @param polarity 捕获极性，可选值: [TIM_ICPOLARITY_RISING, TIM_ICPOLARITY_FALLING, TIM_ICPOLARITY_BOTHEDGE]
 */
void BSP_Timer_InputCapture_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period, uint32_t channel, uint32_t polarity)
{
    TIM_IC_InitTypeDef TIM_IC_InitStruct = {0};

    htim->Instance = TIMx;                                                      // 定时器寄存器基地址
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;                                // 计数模式
    htim->Init.Prescaler = prescaler;                                           // 预分频系数
    htim->Init.Period = period;                                                 // 自动重装载值
    HAL_TIM_IC_Init(htim);

    TIM_IC_InitStruct.ICPolarity = polarity;                                    // 捕获极性
    TIM_IC_InitStruct.ICSelection = TIM_ICSELECTION_DIRECTTI;                   // 直接映射
    TIM_IC_InitStruct.ICPrescaler = TIM_ICPSC_DIV1;                             // 1分频
    TIM_IC_InitStruct.ICFilter = 0;                                             // 不滤波
    HAL_TIM_IC_ConfigChannel(htim, &TIM_IC_InitStruct, channel);
}

/**
 * @brief 定时器脉冲计数功能初始化函数
 * 
 * @param htim 定时器句柄
 * @param TIMx 定时器寄存器基地址，可选值: TIMx, x可选范围: 1 ~ 5, 8 ~ 14
 * @param prescaler 预分频系数，可选值: 0 ~ 65535
 * @param period 自动重装载值，可选值: 0 ~ 65535
 * @param trigger 输入触发源，可选值: [TIM_TS_TI1F_ED, TIM_TS_TI1FP1, TIM_TS_TI2FP2]
 * @param polarity 输入触发极性，可选值: [TIM_TRIGGERPOLARITY_FALLING, TIM_TRIGGERPOLARITY_RISING, TIM_TRIGGERPOLARITY_BOTHEDGE]
 */
void BSP_Timer_PluseCount_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period, uint32_t trigger, uint32_t polarity)
{
    TIM_SlaveConfigTypeDef TIM_SlaveConfigStruct = {0};

    htim->Instance = TIMx;                                                      // 定时器寄存器基地址
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;                                // 计数模式
    htim->Init.Prescaler = prescaler;                                           // 预分频系数
    htim->Init.Period = period;                                                 // 自动重装载值
    HAL_TIM_IC_Init(htim);

    TIM_SlaveConfigStruct.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;                  // 外部触发模式1
    TIM_SlaveConfigStruct.InputTrigger = trigger;                               // 输入触发源
    TIM_SlaveConfigStruct.TriggerPolarity = polarity;                           // 输入触发极性
    TIM_SlaveConfigStruct.TriggerPrescaler = TIM_ICPSC_DIV1;                    // 输入触发预分频
    TIM_SlaveConfigStruct.TriggerFilter = 0;                                    // 输入滤波器设置
    HAL_TIM_SlaveConfigSynchro(htim, &TIM_SlaveConfigStruct);
}

/**
 * @brief 定时器PWM功能初始化函数
 * 
 * @param htim 定时器句柄
 * @param TIMx 定时器寄存器基地址，可选值: TIMx, x可选范围: 1 ~ 5, 8 ~ 14
 * @param prescaler 预分频系数，可选值: 0 ~ 65535
 * @param period 自动重装载值，可选值: 0 ~ 65535
 * @param trigger 触发源，可选值: [TIM_TS_TI1FP1, TIM_TS_TI2FP2]
 */
void BSP_Timer_PWM_InputCapture_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period,  uint32_t trigger)
{
    TIM_SlaveConfigTypeDef TIM_SlaveConfigStruct = {0};
    TIM_IC_InitTypeDef TIM_IC_InitStruct = {0};

    htim->Instance = TIMx;                                                      // 定时器寄存器基地址
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;                                // 计数模式
    htim->Init.Prescaler = prescaler;                                           // 预分频系数
    htim->Init.Period = period;                                                 // 自动重装载值
    HAL_TIM_IC_Init(htim);

    // 配置触发源触发通道，记录PWM的周期
    TIM_SlaveConfigStruct.SlaveMode = TIM_SLAVEMODE_RESET;                      // 复位模式
    TIM_SlaveConfigStruct.InputTrigger = trigger;                               // 输入触发源
    TIM_SlaveConfigStruct.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;         // 输入触发极性
    TIM_SlaveConfigStruct.TriggerPrescaler = TIM_ICPSC_DIV1;                    // 输入触发预分频
    TIM_SlaveConfigStruct.TriggerFilter = 0;                                    // 输入滤波器设置
    HAL_TIM_SlaveConfigSynchro(htim, &TIM_SlaveConfigStruct);

    // 通道1上升沿捕获，记录PWM的周期
    TIM_IC_InitStruct.ICPolarity = TIM_ICPOLARITY_RISING;                       // 上升沿捕获
    TIM_IC_InitStruct.ICPrescaler = TIM_ICPSC_DIV1;                             // 1分频
    TIM_IC_InitStruct.ICFilter = 0;                                             // 不滤波
    if (trigger == TIM_TS_TI1FP1)
    {
        TIM_IC_InitStruct.ICSelection = TIM_ICSELECTION_DIRECTTI;               // 直接映射
    }
    else if (trigger == TIM_TS_TI2FP2)
    {
        TIM_IC_InitStruct.ICSelection = TIM_ICSELECTION_INDIRECTTI;             // 间接映射
    }
    HAL_TIM_IC_ConfigChannel(htim, &TIM_IC_InitStruct, TIM_CHANNEL_1);
  
    // 通道2下降捕获，记录PWM高电平的时间
    TIM_IC_InitStruct.ICPolarity = TIM_ICPOLARITY_FALLING;                      // 下降沿捕获
    if (trigger == TIM_TS_TI1FP1)
    {
        TIM_IC_InitStruct.ICSelection = TIM_ICSELECTION_INDIRECTTI;             // 间接映射
    }
    else if (trigger == TIM_TS_TI2FP2)
    {
        TIM_IC_InitStruct.ICSelection = TIM_ICSELECTION_DIRECTTI;               // 直接映射
    }
    HAL_TIM_IC_ConfigChannel(htim, &TIM_IC_InitStruct, TIM_CHANNEL_2);
}

/**
 * @brief 定时器输入捕获底层初始化函数
 * 
 * @param htim 定时器句柄
 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (htim->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();                                            // 使能TIM2的时钟
        __HAL_RCC_GPIOA_CLK_ENABLE();                                           // 使能TIM2的Channel 1对应的GPIO时钟

        GPIO_InitStruct.Pin = GPIO_PIN_5;                                       // TIM2的Channel 1对应的GPIO引脚
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                                 // 复用功能
        GPIO_InitStruct.Pull = GPIO_NOPULL;                                     // 使用下拉
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;                              // 复用功能选择
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    } 
    else if (htim->Instance == TIM5)
    {
        __HAL_RCC_TIM5_CLK_ENABLE();                                            // 使能TIM5的时钟
        __HAL_RCC_GPIOA_CLK_ENABLE();                                           // 使能TIM5的Channel 1对应的GPIO时钟

        GPIO_InitStruct.Pin = GPIO_PIN_0;                                       // TIM5的Channel 1对应的GPIO引脚
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                                 // 复用功能
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;                                   // 使用下拉
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;                              // 复用功能选择
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(TIM5_IRQn);                                          // 使能定时器5中断
        HAL_NVIC_SetPriority(TIM5_IRQn, 4, 0);                                  // 设置中断优先级
    } 
}

/**
 * @brief 定时器输出比较功能初始化函数
 * 
 * @param htim 定时器句柄
 * @param TIMx 定时器寄存器基地址，可选值: TIMx, x可选范围: 1 ~ 5, 8 ~ 14
 * @param prescaler 预分频系数，可选值: 0 ~ 65535
 * @param period 自动重装载值，可选值: 0 ~ 65535
 * @param channel 输出PWM的通道，可选值: TIM_CHANNEL_x, x可选范围: 1 ~ 4
 * @param polarity 输出比较极性，可选值: [TIM_OCPOLARITY_LOW, TIM_OCPOLARITY_HIGH]
 * @param pluse 输出比较值，可选值: 0 ~ 65535
 */
void BSP_Timer_OutputCompare_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period, uint32_t channel, uint32_t polarity, uint32_t pulse)
{
    TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

    htim->Instance = TIMx;                                                      // 定时器寄存器基地址
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;                                // 计数模式
    htim->Init.Prescaler = prescaler;                                           // 预分频系数
    htim->Init.Period = period;                                                 // 自动重装载值
    HAL_TIM_OC_Init(htim);

    TIM_OC_InitStruct.OCMode = TIM_OCMODE_TOGGLE;                               // 输出比较模式翻转功能
    TIM_OC_InitStruct.OCPolarity = polarity;                                    // 输出比较极性
    TIM_OC_InitStruct.Pulse = pulse;                                            // 比较值
    HAL_TIM_OC_ConfigChannel(htim, &TIM_OC_InitStruct, channel);
}

/**
 * @brief 定时器输出比较底层初始化函数
 * 
 * @param htim 定时器句柄
 */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (htim->Instance == TIM13)
    {
        __HAL_RCC_TIM13_CLK_ENABLE();                                           // 使能TIM13的时钟
        __HAL_RCC_GPIOF_CLK_ENABLE();                                           // 使能TIM13的Channel 1对应的GPIO时钟

        GPIO_InitStruct.Pin = GPIO_PIN_8;                                       // TIM13的Channel 1对应的GPIO引脚
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                                 // 复用功能
        GPIO_InitStruct.Pull = GPIO_NOPULL;                                     // 不使用上下拉
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                           // GPIO输出速度
        GPIO_InitStruct.Alternate = GPIO_AF9_TIM13;                             // 复用功能选择
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    }
}

/**
 * @brief PWM DAC函数
 * 
 * @param htim 定时器句柄
 * @param channel 定时器通道
 * @param voltage 电压值
 */
void BSP_PWM_DAC_SetVoltage(TIM_HandleTypeDef *htim, uint32_t channel, double voltage)
{
    voltage = voltage * 256 / 3.3;
    __HAL_TIM_SET_COMPARE(htim, channel, voltage);
}

/**
 * @brief 定时器获取脉冲计数函数
 * 
 * @param htim 定时器句柄
 * @param updateCount 定时器更新次数
 * @return uint32_t 脉冲数
 */
uint32_t BSP_Timer_GetPluseCount(TIM_HandleTypeDef *htim, uint16_t updateCount)
{
    uint32_t count = 0;
    count = updateCount * 65535;                                                // 计算溢出次数对应的计数值
    count += __HAL_TIM_GET_COUNTER(htim);                                       // 计算当前的计数值
    return count;
}

/**
 * @brief 定时器8设置PWM输出次数函数
 * 
 * @param remain_npwm 要设置的PWM输出次数剩余值
 * @param npwm 要设置的PWM输出次数
 */
void BSP_Timer_NPWM_SetCount(TIM_HandleTypeDef *htim, uint32_t *pRemain_npwm, uint32_t npwm)
{ 
    *pRemain_npwm = npwm;                                                       // 保存脉冲个数
    HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE);                        //  产生一次更新事件，在中断里面处理脉冲输出
    __HAL_TIM_ENABLE(htim);                                                     // 使能定时器
}

/**
 * @brief 定时器脉冲计数清零函数
 * 
 * @param htim 定时器句柄
 * @param updateCount 定时器更新次数
 */
void BSP_Timer_PluseCount_Reset(TIM_HandleTypeDef *htim, uint16_t *pUpdateCount)
{
    __HAL_TIM_DISABLE(htim);                                                    // 关闭定时器
    *pUpdateCount = 0;                                                          // 累加值清零
    __HAL_TIM_SET_COUNTER(htim, 0);                                             // 计数器清零
    __HAL_TIM_ENABLE(htim);                                                     // 使能定时器
}

/**
 * @brief 定时器PWM输入模式状态清零函数
 * 
 * @param status PWM的捕获状态
 * @param highLevelDuration 高电平持续时间
 * @param period PWM的周期周期
 */
void BSP_Timer_PWM_InputCapture_Reset(uint8_t *pStatus, uint16_t *pHighLevelDuration, uint16_t *pPeriod)
{
    __ASM volatile("cpsid i");                                                  // 关闭所有中断
  
    // 清零状态，重新开始检测
    *pStatus = 0;
    *pHighLevelDuration = 0;
    *pPeriod = 0;

    __ASM volatile("cpsie i");                                                  // 开启所有中断
}

/**
 * @brief 定时器5中断服务函数
 * 
 */
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timer5_handle);                                       // 调用HAL库公共处理函数
}

/**
 * @brief 定时器6中断服务函数
 * 
 */
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timer6_handle);                                       // 调用HAL库公共处理函数
}

/**
 * @brief 定时器8到13更新中断服务函数
 * 
 */
void TIM8_UP_TIM13_IRQHandler(void)
{
    uint16_t npwm = 0;

    if (__HAL_TIM_GET_FLAG(&g_timer8_handle, TIM_FLAG_UPDATE) != RESET)
    {
        if (g_timer8_channel1_npwm_remain >= 256)                               // 还有大于等于256个脉冲需要发送
        {
            g_timer8_channel1_npwm_remain -= 256;
            npwm = 256;
        }
        else if (g_timer8_channel1_npwm_remain % 256)                           //  还有位数（不到 256）个脉冲要发送
        {
            npwm = g_timer8_channel1_npwm_remain % 256;
            g_timer8_channel1_npwm_remain = 0;                                  //  没有脉冲了
        }
  
        if (npwm)                                                               // 有脉冲要发送
        {
            TIM8->RCR = npwm - 1;                                               // 设置RCR值为npwm-1，即npwm个脉冲
            HAL_TIM_GenerateEvent(&g_timer8_handle, TIM_EVENTSOURCE_UPDATE);    // 产生一次更新事件，以更新RCR寄存器
            __HAL_TIM_ENABLE(&g_timer8_handle);                                 // 使能定时器
        }
        else
        {
            TIM8->CR1 &= ~(1 << 0);                                             // 关闭定时器 TIMX
        }
    }

    __HAL_TIM_CLEAR_IT(&g_timer8_handle, TIM_IT_UPDATE);                        // 清除定时器更新中断标志位
}

/**
 * @brief 定时器更新中断回调函数
 * 
 * @param htim 定时器句柄
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM5)
    {
        if ((g_timer5_channel1_capture_status & 0x80) == 0)                     // 还没有成功捕获
        {
            if (g_timer5_channel1_capture_status & 0x40)                        // 已经捕获到了高电平
            {
                if ((g_timer5_channel1_capture_status & 0x3F) == 0x3F)          // 高电平时间太长了
                {
                    // 清除原来的设置
                    TIM_RESET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1);
                    // 重新配置TIM5的通道1上升沿捕获
                    TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);

                    g_timer5_channel1_capture_status |= 0x80;
                    g_timer5_channel1_capture_value = 0xFFFF;
                }
                else
                {
                    g_timer5_channel1_capture_status++;                         // 累计定时器溢出次数
                }
            }
        }
    }
    else if (htim->Instance == TIM6)
    {
        extern void DHCP_time_handler(void);
        DHCP_time_handler();
        extern void httpServer_time_handler(void);
        httpServer_time_handler();

        // g_timer_1ms_ticks++;
    }
}

/**
 * @brief 定时器输入捕获回调函数
 * 
 * @param htim 定时器句柄
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM5)
    {
        if (g_timer5_channel1_pwm_in_status == 0)
        {
            if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)                      // 当前通道为通道1
            {
                // 获取高电平的计数值
                g_timer5_channel1_pwm_in_high_level_duration = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2) + 1;
                // 获取PWM周期的计数值
                g_timer5_channel1_pwm_in_period = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1) + 1;
                g_timer5_channel1_pwm_in_status = 1;                            // 成功获取
            }  
        }
    }
}