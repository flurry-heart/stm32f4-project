#ifndef __BSP_DMA_H__
#define __BSP_DMA_H__

#include "stm32f4xx_hal.h"

extern DMA_HandleTypeDef g_dma1_handle;
extern DMA_HandleTypeDef g_dma2_handle;
extern uint8_t dma2_stream0_is_finished;

void BSP_DMA_MemoryToMemory_Init(DMA_HandleTypeDef *hdma, DMA_Stream_TypeDef *dma_stream, uint32_t channel, uint8_t dataLength, uint32_t mode, uint32_t priority);
void BSP_DMA_MemoryToPeripheral_Init(DMA_HandleTypeDef *hdma, DMA_Stream_TypeDef *dma_stream, uint32_t channel, uint8_t dataLength, uint32_t mode, uint32_t priority);
void BSP_DMA_PeripheralToMemory_Init(DMA_HandleTypeDef *hdma, DMA_Stream_TypeDef *dma_stream, uint32_t channel, uint8_t dataLength, uint32_t mode,  uint32_t priority);

void DMA_CompleteCallback(DMA_HandleTypeDef *hdma);

#endif // !__BSP_DMA_H__