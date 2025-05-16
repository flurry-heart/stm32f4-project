#include "bsp_iap.h"

typedef void (*iap_fun)(void);                                                  // 定义一个函数类型的参数

uint32_t g_iap_buffer[512];                                                     // 2K字节缓存
iap_fun jumpToApp;

/**
 * @brief 将固件写入到FLASH中
 * 
 * @param address 应用程序的起始地址
 * @param data 执行应用程序bin文件的指针
 * @param length 应用程序大小（字节数）
 */
void BSP_IAP_WriteAppBin(uint32_t address, uint8_t *data, uint32_t length)
{
    uint32_t temp = 0;
    uint8_t *current_data = data;
    uint32_t k = 0;
    uint32_t current_address = address;

    for (uint32_t i = 0; i < length; i += 4)
    {
        // 拼接数据
        temp = (uint32_t)current_data[3] << 24;
        temp |= (uint32_t)current_data[2] << 16;
        temp |= (uint32_t)current_data[1] << 8;
        temp |= (uint32_t)current_data[0];
        current_data += 4;
        g_iap_buffer[k++] = temp;

        if (k == 512)
        {
            k = 0;
            BSP_FLASH_WriteData(current_address, g_iap_buffer, 512);
            current_address += 2048;                                            // 偏移2K字节
        }
    }
    if (k)
    {
        BSP_FLASH_WriteData(current_address, g_iap_buffer, k);                  // 将最后剩余的字节写入
    }
}

/**
 * @brief 跳转到应用程序段(执行APP)
 * 
 * @param address 应用程序的起始地址
 */
void BSP_IAP_LoadApp(uint32_t address)
{
    printf("%#lX\r\n", (*(volatile uint32_t *)address));
    // 栈顶检查没有通过，栈顶地址为: 0X20020000
    // if (((*(volatile uint32_t *)address) & 0x2FFE0000) == 0x20000000)          // 检查栈顶地址是否合法.可以放在内部SRAM共128KB(0x20000000)
    {
        jumpToApp = (iap_fun)*(volatile uint32_t *)(address + 4);               // 用户代码区第二个字为程序开始地址(复位地址)
      
        // 初始化APP堆栈指针（用户代码区的第一个字用于存放栈顶地址）
        __set_MSP(address);                                                     // 设置栈顶地址
        jumpToApp();                                                            // 跳转到APP
    }
}