#include "system.h"

/**
 * @brief 设置中断向量表偏移地址
 * 
 * @param baseAddress 基地址
 * @param offset 偏移量
 */
void System_NVIC_SetVectorTable(uint32_t baseAddress, uint32_t offset) 
{
    // 设置NVIC的向量表偏移寄存器，VTOR低9位保留，即[8:0]保留
    SCB->VTOR = baseAddress | (offset & (uint32_t)0xFFFFFE00);
}