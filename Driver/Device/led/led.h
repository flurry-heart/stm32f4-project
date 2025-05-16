#ifndef __LED_H__
#define __LED_H__

#include "stm32f4xx_hal.h"

typedef enum LED_State
{
    LED_ON,
    LED_OFF,
} LED_State;

void LED_Init(void);
void LED_SetStatus(GPIO_TypeDef  *LED_Port, uint16_t LED_Pin, LED_State status);

#endif // !__LED_H__