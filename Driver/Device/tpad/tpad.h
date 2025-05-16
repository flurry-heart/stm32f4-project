#ifndef __TPAD_H__
#define __TPAD_H__

#include "bsp_timer.h"

#include "sort/sort.h"

#define TPAD_MAX_VALUE  0xFFFF
#define TPAD_GATE_VALUE 10                                                      // 修改此值，控制电容触摸按键灵敏度

void TPad_Init(TIM_HandleTypeDef *htim);

#endif // !__TPAD_H__