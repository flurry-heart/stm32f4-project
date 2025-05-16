#include "bsp_rtc.h"

RTC_HandleTypeDef g_rtc_handle;

/**
 * @brief RTC初始化函数
 * 
 * @param dateTime DateTime，日期时间结构体
 */
void BSP_RTC_Init(DateTime_t dateTime)
{
    uint16_t flag = 0;

    g_rtc_handle.Instance = RTC;                                                // RTC寄存器基地址
    g_rtc_handle.Init.HourFormat = RTC_HOURFORMAT_24;                           // 小时格式
    g_rtc_handle.Init.AsynchPrediv = 0x7F;                                      // 异步预分频系数
    g_rtc_handle.Init.SynchPrediv = 0xFF;                                       // 同步预分频系数
    g_rtc_handle.Init.OutPut = RTC_OUTPUT_DISABLE;                              // 选择连接到RTC_ALARM输出的标志
    g_rtc_handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;                // 设置RTC_ALARM的输出极性
    g_rtc_handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;                   // 设置RTC_ALARM的输出类型为开漏输出还是推挽输出
    HAL_RTC_Init(&g_rtc_handle);


    flag = HAL_RTCEx_BKUPRead(&g_rtc_handle, 0);                                // 读取后备域寄存器0的值
    if (flag != 0x5050)                                                         // 之前未初始化过, 重新配置
    {
        BSP_RTC_SetDateTime(dateTime);                                          // 设置RTC时间
        HAL_RTCEx_BKUPWrite(&g_rtc_handle, 0, 0x5050);                          // 写入0x5050到后备域寄存器0
    }
}

/**
 * @brief RTC底层初始化回调函数
 * 
 * @param hrtc RTC的句柄
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();                                                 // 使能PWR时钟
    HAL_PWR_EnableBkUpAccess();                                                 // 使能备份域访问
  
    __HAL_RCC_RTC_ENABLE();                                                     // 使能RTC

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;                  // 选择时钟源
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;                                    // 开启LSE
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;                              // 不配置PLL
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;           // 选择RTC时钟
    RCC_PeriphCLKInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;           // RTC时钟源来自LSE
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
}

/**
 * @brief RTC设置时间
 * 
 * @param time Time，时间的结构体
 */
void BSP_RTC_SetTime(Time_t time)
{
    RTC_TimeTypeDef RTC_TimeStruct = {0};

    RTC_TimeStruct.Hours = time.hour;                                           // 时
    RTC_TimeStruct.Minutes = time.minute;                                       // 分
    RTC_TimeStruct.Seconds = time.second;                                       // 秒
    RTC_TimeStruct.TimeFormat = RTC_HOURFORMAT_24;                              // 24小时制
    RTC_TimeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;                    // 不使用夏令时
    RTC_TimeStruct.StoreOperation = RTC_STOREOPERATION_RESET;                   // RTC不支持自动重置
    HAL_RTC_SetTime(&g_rtc_handle, &RTC_TimeStruct, RTC_FORMAT_BIN); 
}

/**
 * @brief RTC设置日期函数
 * 
 * @param date Date，日期的结构体
 */
void BSP_RTC_SetDate(Date_t date)
{
    RTC_DateTypeDef RTC_DateStruct = {0};

    RTC_DateStruct.Year = date.year - 2000;                                     // 年
    RTC_DateStruct.Month = date.month;                                          // 月
    RTC_DateStruct.Date = date.day;                                             // 日
    RTC_DateStruct.WeekDay = date.weekDay;                                      // 周
    HAL_RTC_SetDate(&g_rtc_handle, &RTC_DateStruct, RTC_FORMAT_BIN);
}

/**
 * @brief RTC设置日期时间函数
 * 
 * @param dateTime DateTime，日期时间的结构体
 */
void BSP_RTC_SetDateTime(DateTime_t dateTime)
{
    BSP_RTC_SetTime(dateTime.time);
    BSP_RTC_SetDate(dateTime.date);
}

/**
 * @brief RTC设置闹钟函数
 * 
 * @param dateTime 日期时间结构体
 */
void BSP_RTC_SetAlarm(DateTime_t dateTime)
{
    RTC_AlarmTypeDef RTC_AlarmStruct = {0};

    RTC_AlarmStruct.AlarmTime.Hours = dateTime.time.hour;                       // 时
    RTC_AlarmStruct.AlarmTime.Minutes = dateTime.time.minute;                   // 分
    RTC_AlarmStruct.AlarmTime.Seconds = dateTime.time.second;                   // 秒
    RTC_AlarmStruct.AlarmTime.SubSeconds = 0;                                   // 亚秒
    RTC_AlarmStruct.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;                   // 24小时制

    RTC_AlarmStruct.AlarmDateWeekDay = dateTime.date.weekDay;                   // 星期

    RTC_AlarmStruct.AlarmMask = RTC_ALARMMASK_NONE;                             // 精确匹配星期，时分秒
    RTC_AlarmStruct.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    RTC_AlarmStruct.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;      // 按星期

    RTC_AlarmStruct.Alarm = RTC_ALARM_A;                                        // 闹钟A
    HAL_RTC_SetAlarm_IT(&g_rtc_handle, &RTC_AlarmStruct, RTC_FORMAT_BIN);
  
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

/**
 * @brief RTC周期性唤醒函数
 * 
 * @param clock RTC时钟频率，可选值：
 *                              RTC_WAKEUPCLOCK_RTCCLK_DIV1x ，x可取[2, 4, 8, 16]
 *                              RTC_WAKEUPCLOCK_CK_SPRE_16BITS
 *                              RTC_WAKEUPCLOCK_CK_SPRE_16BITS
 * @param count 自动重装载值，减到0，产生中断
 */
void BSP_RTC_SetWakeUp(uint8_t clock, uint16_t count)
{ 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_WUTF);             // 清除RTC WAKE UP的标志

    HAL_RTCEx_SetWakeUpTimer_IT(&g_rtc_handle, count, clock);                   // 设置重装载值和时钟

    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

/**
 * @brief RTC闹钟中断服务函数
 * 
 */
void RTC_Alarm_IRQHandler(void)
{
    if (__HAL_RTC_ALARM_GET_FLAG(&g_rtc_handle, RTC_FLAG_ALRAF) != 0U)
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);

        __HAL_RTC_ALARM_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_ALRAF);
    }
}

/**
 * @brief RTC周期性唤醒服务函数
 * 
 */
void RTC_WKUP_IRQHandler(void)
{
    if (__HAL_RTC_WAKEUPTIMER_GET_FLAG(&g_rtc_handle, RTC_FLAG_WUTF) != 0U)
    {
        HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
        __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_WUTF);         // 清除唤醒定时器中断挂起位
    }

    __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();                                    // 为RTC WakeUpTimer清除EXTI标志

    g_rtc_handle.State = HAL_RTC_STATE_READY;                                   // 改变RTC状态
}