#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef g_timer1_handle;
extern TIM_HandleTypeDef g_timer2_handle;
extern TIM_HandleTypeDef g_timer5_handle;
extern TIM_HandleTypeDef g_timer6_handle;
extern TIM_HandleTypeDef g_timer8_handle;
extern TIM_HandleTypeDef g_timer11_handle;
extern TIM_HandleTypeDef g_timer13_handle;
extern TIM_HandleTypeDef g_timer14_handle;

extern uint8_t g_timer5_channel1_capture_status;                                // 定时器5通道1的输入捕获状态
extern uint16_t g_timer5_channel1_capture_value;                                // 定时器5通道1的输入捕获值
extern uint16_t g_timer5_channel1_update_count;                                 // 定时器更新计数值

extern uint8_t g_timer5_channel1_pwm_in_status;                                 // 0: 未检测到PWM信号; 1: 检测到PWM信号
extern uint16_t g_timer5_channel1_pwm_in_period;                                // PWM周期
extern uint16_t g_timer5_channel1_pwm_in_high_level_duration;                   // 高电平持续时间

extern uint32_t g_timer8_channel1_npwm_remain;

extern uint32_t g_timer_1ms_ticks;

void BSP_Timer_Base_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period);

void BSP_Timer_PWM_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period, uint32_t channel, uint32_t polarity, uint32_t pluse);
void BSP_Timer_OutputCompare_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period, uint32_t channel, uint32_t polarity, uint32_t pulse);
void BSP_Timer_ComplementaryOutput_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period, uint32_t channel, uint32_t polarity, uint32_t NPolarity, uint32_t pluse, uint32_t deadTime, uint32_t breakPolarity);

void BSP_Timer_InputCapture_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period, uint32_t channel, uint32_t polarity);
void BSP_Timer_PluseCount_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period, uint32_t trigger, uint32_t polarity);
void BSP_Timer_PWM_InputCapture_Init(TIM_HandleTypeDef *htim, TIM_TypeDef *TIMx, uint16_t prescaler, uint16_t period,  uint32_t trigger);

void BSP_PWM_DAC_SetVoltage(TIM_HandleTypeDef *htim, uint32_t channel, double voltage);

uint32_t BSP_Timer_GetPluseCount(TIM_HandleTypeDef *htim, uint16_t updateCount);

void BSP_Timer_NPWM_SetCount(TIM_HandleTypeDef *htim, uint32_t *pRemain_npwm, uint32_t npwm);

void BSP_Timer_PluseCount_Reset(TIM_HandleTypeDef *htim, uint16_t *pUpdateCount);
void BSP_Timer_PWM_InputCapture_Reset(uint8_t *pStatus, uint16_t *pHighLevelDuration, uint16_t *pPeriod);

#endif // !__BSP_TIMER_H__