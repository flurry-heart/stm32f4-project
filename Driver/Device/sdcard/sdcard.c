#include "sdcard.h"

SD_HandleTypeDef g_sd_handler;

/**
 * @brief SD卡初始化函数
 * 
 * @return uint8_t 0: 代表成功
 */
uint8_t SD_Init(void)
{
    // 初始化时的时钟不能大于400KHZ， SD传输时钟频率最大 25MHZ
    g_sd_handler.Instance = SDIO;
    g_sd_handler.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;                       // 数据或指令变化的时钟沿
    g_sd_handler.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;                  // 不设置旁路分频器，直接用 HCLK 进行分频得到SDIO_CK
    g_sd_handler.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;           // 空闲时不关闭时钟电源
    g_sd_handler.Init.BusWide = SDIO_BUS_WIDE_1B;                               // SDIO总线位宽
    g_sd_handler.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE; // 使能硬件流控制
    g_sd_handler.Init.ClockDiv = 1;                                             // 设置SDIO时钟分频
    return HAL_SD_Init(&g_sd_handler);
}

/**
 * @brief SD卡底层初始化函数
 * 
 * @param hsd SDIO句柄
 */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hsd->Instance == SDIO)
    {
        __HAL_RCC_SDIO_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_2;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }
}

/**
 * @brief SD卡块读取数据函数
 * 
 * @param hsd SDIO句柄
 * @param blockAddress 扇区地址
 * @param numberOfBlocks 读取的扇区个数
 * @param buffer 保存读取数据的缓冲区
 * 
 * @return uint8_t 读取成功返回0，否则返回1
 */
uint8_t SD_ReadData(SD_HandleTypeDef *hsd, uint32_t blockAddress, uint32_t blockCount, uint8_t *buffer)
{
    uint8_t status = 0;
    uint32_t timeOut = 1000;
    HAL_StatusTypeDef sd_status = HAL_OK;

    __disable_irq();                                                            // 关闭总中断（POLLIN模式，严禁中断打断SDIO读写操作

    sd_status = HAL_SD_ReadBlocks(hsd, buffer, blockAddress, blockCount, 0xffff);
    status = (sd_status == HAL_OK) ? 0 : 1;

    // 等待SD卡读完
    while (HAL_SD_GetCardState(hsd) != HAL_SD_CARD_TRANSFER)
    {
        status = (timeOut--) ? status : 2;
    }

    __enable_irq();                                                             // 开启总中断

    return status;
}

/**
 * @brief SD卡块写入数据函数
 * 
 * @param hsd SDIO句柄
 * @param sectorAddress 扇区地址
 * @param buffer 写入数据的缓冲区
 * @param sectorCount 写入的扇区个数
 * 
 * @return uint8_t 成功写入返回0，否则返回1
 */
uint8_t SD_WriteData(SD_HandleTypeDef *hsd, uint32_t blockAddress, uint32_t blockCount, uint8_t *buffer)
{
    uint8_t status = 0;
    uint32_t timeOut = 1000;
    HAL_StatusTypeDef sd_status = HAL_OK;

    __disable_irq();                                                            // 关闭总中断（POLLIN模式，严禁中断打断SDIO读写操作

    sd_status = HAL_SD_WriteBlocks(hsd, buffer, blockAddress, blockCount, 0xffff);
    status = (sd_status == HAL_OK) ? 0 : 1;

    // 等待SD卡写完
    while (HAL_SD_GetCardState(hsd) != HAL_SD_CARD_TRANSFER)
    {
        status = (timeOut--) ? status : 2;
    }

    __enable_irq();                                                             // 开启总中断

    return status;
}