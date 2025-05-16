#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <string.h>

#define SRAM_MEMORY_POOL_SIZE           10 * 1024
#define SRAM_MEMORY_POOL_BLOCK_SIZE     32
#define SRAM_MEMORY_POOL_BLOCK_COUNT    SRAM_MEMORY_POOL_SIZE / SRAM_MEMORY_POOL_BLOCK_SIZE

typedef struct Memory_t
{
    uint8_t *pool;
    uint16_t *table;
    uint32_t pool_size;
    uint32_t block_size;
    uint32_t block_count;
    uint32_t used_block_count;
} memory_t;

extern memory_t g_sram_memory;
extern __attribute((aligned (64))) uint8_t g_sram_memory_pool[SRAM_MEMORY_POOL_SIZE];
extern uint16_t g_srammemory_table[SRAM_MEMORY_POOL_BLOCK_COUNT];

void memory_init(memory_t *memory, uint8_t *pool, uint16_t *table, uint32_t pool_size, uint32_t block_size);
void memory_set_value(void *ptr, uint8_t value, uint32_t size);
void memory_copy(void *des, void *src, uint32_t n);

void * memory_malloc(memory_t *memory, uint32_t size);
void * memory_realloc(memory_t *memory, void *ptr, uint32_t size);
void memory_free(memory_t *memory, void *ptr);

#endif // !__MEMORY_H__