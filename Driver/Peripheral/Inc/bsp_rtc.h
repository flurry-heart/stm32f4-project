#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__

#include "stm32f4xx_hal.h"

typedef struct Date_t
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekDay;
} Date_t;

typedef struct Time_t
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} Time_t;

typedef struct DateTime_t
{
    Date_t date;
    Time_t time;
} DateTime_t;

extern RTC_HandleTypeDef g_rtc_handle;

void BSP_RTC_Init(DateTime_t dateTime);
void BSP_RTC_SetTime(Time_t time);
void BSP_RTC_SetDate(Date_t date);
void BSP_RTC_SetDateTime(DateTime_t dateTime);
void BSP_RTC_SetAlarm(DateTime_t dateTime);
void BSP_RTC_SetWakeUp(uint8_t clock, uint16_t count);

#endif // !__BSP_RTC_H__