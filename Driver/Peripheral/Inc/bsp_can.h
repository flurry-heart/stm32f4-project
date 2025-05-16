#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

#include "stm32f4xx_hal.h"

extern CAN_HandleTypeDef g_can1_handle;

void BSP_CAN_Init(CAN_HandleTypeDef *hcan, CAN_TypeDef *CANx, uint32_t mode, uint32_t BKP, uint32_t TS1, uint32_t TS2, uint32_t sjw);
void BSP_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, uint32_t mode, uint32_t scale, uint32_t id, uint32_t maskId, uint32_t bank, uint32_t fifo);
void BSP_CAN_SendMessage(CAN_HandleTypeDef *hcan, uint32_t id, uint32_t mail, uint8_t *data, uint8_t length);
uint8_t BSP_CAN_ReceiveMessage(CAN_HandleTypeDef *hcan, uint32_t fifo, uint8_t *data);

#endif // !__BSP_CAN_H__