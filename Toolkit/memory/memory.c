#include "memory.h"

memory_t g_sram_memory;
__attribute((aligned (64))) uint8_t g_sram_memory_pool[SRAM_MEMORY_POOL_SIZE];
uint16_t g_srammemory_table[SRAM_MEMORY_POOL_BLOCK_COUNT];

/**
 * @brief 内存管理初始化
 * 
 * @param memory 要管理的内存
 * @param pool 内存池的地址
 * @param table 内存表的地址
 * @param pool_size 内存池的大小
 * @param block_size 单个内存块的大小
 */
void memory_init(memory_t *memory, uint8_t *pool, uint16_t *table, uint32_t pool_size, uint32_t block_size)
{
    uint32_t block_count = pool_size / block_size;

    memory_set_value(pool, 0, pool_size);
    memory_set_value(table, 0, block_count * 2);

    memory->pool = pool;
    memory->table = table;
    memory->pool_size = pool_size;
    memory->block_size = block_size;
    memory->block_count = block_count;
    memory->used_block_count = 0;
}

/**
 * @brief 设置内存的值
 * 
 * @param ptr 要设置的内存的地址
 * @param value 要设置的值
 * @param size 要设置的内存的字节数
 */
void memory_set_value(void *ptr, uint8_t value, uint32_t size)
{
    uint8_t *p = ptr;

    while (size--)
    {
        *p++ = value;
    }
}

/**
 * @brief 复制内存
 * 
 * @param des 目的地址
 * @param src 源地址
 * @param n 复制的字节数
 */
void memory_copy(void *des, void *src, uint32_t n)
{
    uint8_t *xdes = des;
    uint8_t *xsrc = src;

    while (n--) 
    {
        *xdes++ = *xsrc++;
    }
}

/**
 * @brief 申请内存
 * 
 * @param memory 要管理的内存
 * @param size 要申请的字节数
 * @return void* 申请的内存块的地址
 */
void * memory_malloc(memory_t *memory, uint32_t size)
{
    uint32_t connected_memory_block = 0;

    if (size <= 0)
    {
        return NULL;
    }
  
    // 需要的内存块数
    uint32_t block_count = size % memory->block_size ? size / memory->block_size + 1 : size / memory->block_size;
  
    // 从表尾往头寻找空闲块
    for (uint32_t offset = memory->block_count - 1; offset >= 0; offset--)
    {
        // 找到空闲块，则相连的内存块数加一，否则清零
        connected_memory_block = (memory->table[offset]) ? 0 : connected_memory_block + 1;
     
        if (connected_memory_block == block_count)
        {
            // 连续块数满足，则标记为已使用
            for (uint32_t i = 0; i < block_count; i++)
            {
                memory->table[offset + i] = connected_memory_block;
            }

            // 返回内存块的地址
            return (void *)((uint32_t)memory->pool + offset * memory->block_size);
        }
    }

    return NULL;
}

/**
 * @brief 重新分配内存函数
 * 
 * @param memory 要管理的内存
 * @param ptr 旧内存首地址
 * @param size 要分配的内存大小(字节)
 * @return void* 新分配到的内存首地址
 */
void * memory_realloc(memory_t *memory, void *ptr, uint32_t size)
{
    uint32_t *offset = 0;
    
    offset = memory_malloc(memory, size);

    if (offset == NULL)                                                         // 申请出错，返回NULL
    {
        return NULL;
    }
    else                                                                        // 申请没问题, 返回首地址
    {
        memory_copy((void *)((uint32_t)memory->pool + offset), ptr, size);      // 拷贝旧内存内容到新内存
        memory_free(memory, ptr);                                               // 释放旧内存
        return (void *)((uint32_t)memory->pool + offset);                       // 返回新内存首地址
    }
}

/**
 * @brief 释放内存
 * 
 * @param memory 要管理的内存
 * @param ptr 要释放的内存块的地址
 */
void memory_free(memory_t *memory, void *ptr)
{
    if ((uint32_t) ptr < (uint32_t)memory->pool || (uint32_t)ptr >= (uint32_t)memory->pool + memory->pool_size)
    {
        return;
    }

    uint32_t offset = (uint32_t)ptr - (uint32_t)memory->pool;                   // 获取内存块地址偏移量
    uint32_t index = offset / memory->block_size;                               // 计算内存块索引
    uint16_t block_count = memory->table[index];                                // 获取占用的内存块数

    // 释放内存块
    for (uint32_t i = 0; i < block_count; i++)
    {
        memory->table[index + i] = 0;
    }
  
    // 清零内存块
    memory_set_value(ptr, 0, block_count * memory->block_size);
}

/**
 * @brief 获取内存使用率
 * 
 * @param memory 要管理的内存
 * @return uint8_t 内存的使用率
 */
uint8_t memory_get_usage_rate(memory_t *memory)
{
    memory->used_block_count = 0;

    for (uint32_t i = 0; i < memory->block_count; i++)
    {
        if (memory->table[i])
        {
            memory->used_block_count++;
        }
    }

    return (memory->used_block_count * 100) / memory->block_count;
}