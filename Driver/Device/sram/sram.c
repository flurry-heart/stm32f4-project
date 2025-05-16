#include "sram.h"

static void FSMC_SRAM_Init(void);
static void SRAM_GPIO_Init(void);

/**
 * @brief SRAMc初始化
 * 
 */
void SRAM_Init(void)
{
    SRAM_GPIO_Init();
    FSMC_SRAM_Init();
}

/**
 * @brief FSMC驱动SRMA初始化
 * 
 */
static void FSMC_SRAM_Init(void)
{
    SRAM_HandleTypeDef hsram = {0};   
    FSMC_NORSRAM_TimingTypeDef FSMC_NORSRAM_TimingStruct = {0};

    hsram.Instance = FSMC_NORSRAM_DEVICE;
    hsram.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;

    hsram.Init.NSBank = (SRAM_FSMC_NEX == 1) ? FSMC_NORSRAM_BANK1 : \
                                 (SRAM_FSMC_NEX == 2) ? FSMC_NORSRAM_BANK2 : \
                                 (SRAM_FSMC_NEX == 3) ? FSMC_NORSRAM_BANK3 : 
                                                        FSMC_NORSRAM_BANK4;     // 根据配置选择FSMC_NE1~4
    hsram.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;                  // 地址/数据线不复
    hsram.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;                              // SRAM
    hsram.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;                 // 16位数据宽度
    hsram.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;                // 是使能突发访问，仅对同步突发存储器有效
    hsram.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;              // 等待信号的极性，仅在突发模式访问下有
    hsram.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;                   // 存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT
    hsram.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;                    // 存储器写使能
    hsram.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;                           // 等待使能位
    hsram.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;                       // 读写使用相同的时序
    hsram.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;               // 是否使能同步传输模式下的等待信号
    hsram.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;                           // 禁止突发写
    hsram.Init.ContinuousClock = FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;

    FSMC_NORSRAM_TimingStruct.AccessMode = FSMC_ACCESS_MODE_A;                  // 模式A
    FSMC_NORSRAM_TimingStruct.AddressSetupTime = 0x00;                          // 地址建立时间（ADDSET
    FSMC_NORSRAM_TimingStruct.AddressHoldTime = 0x00;                           // 地址保持时间（ADDHLD）模式A未用到
    FSMC_NORSRAM_TimingStruct.DataSetupTime = 0x18;                             // 数据保存时间
    FSMC_NORSRAM_TimingStruct.BusTurnAroundDuration = 0x00;

    HAL_SRAM_Init(&hsram, &FSMC_NORSRAM_TimingStruct, &FSMC_NORSRAM_TimingStruct);
}

/**
 * @brief FSMC驱动SRAM底层初始化
 * 
 */
static void SRAM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_FSMC_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /** FSMC GPIO Configuration
        PF0  ------> FSMC_A0    PF1  ------> FSMC_A1    PF2  ------> FSMC_A2
        PF3  ------> FSMC_A3    PF4  ------> FSMC_A4    PF5  ------> FSMC_A5
        PF12 ------> FSMC_A6    PF13 ------> FSMC_A7    PF14 ------> FSMC_A8
        PF15 ------> FSMC_A9  
  
        PG0  ------> FSMC_A10   PG1  ------> FSMC_A11   PG2  ------> FSMC_A12
        PG3  ------> FSMC_A13   PG4  ------> FSMC_A14   PG5  ------> FSMC_A15

        PG10 ------> FSMC_NE3

        PE7  ------> FSMC_D4    PE8  ------> FSMC_D5    PE9  ------> FSMC_D6
        PE10 ------> FSMC_D7    PE11 ------> FSMC_D8    PE12 ------> FSMC_D9
        PE13 ------> FSMC_D10   PE14 ------> FSMC_D11   PE15 ------> FSMC_D12

        PE0 ------> FSMC_NBL0   PE1  ------> FSMC_NBL1

        PD11 ------> FSMC_A16   PD12 ------> FSMC_A17   PD13 ------> FSMC_A18
  
        PD0  ------> FSMC_D2    PD1  ------> FSMC_D3    PD8  ------> FSMC_D13
        PD9  ------> FSMC_D14   PD10 ------> FSMC_D15   PD14 ------> FSMC_D0
        PD15 ------> FSMC_D1

        PD4  ------> FSMC_NOE   PD5  ------> FSMC_NWE   
    */
    // PF0~PF5 PF12~PF15
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | 
                            GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 |
                            GPIO_PIN_14 |GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    // PG0~PG5 PG10
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | 
                            GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    // PE7~PE15 PE0~PE1
    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                            GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                            GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    // PD0~PD1 PD4~PD5 PD8~PD15
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | 
                            GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 |
                            GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/**
 * @brief 往SRAM指定地址写指定长度的数据
 * 
 * @param address 开始写入的地址
 * @param data 数据存储区
 * @param length 写入的字节数
 */
void SRAM_WriteData(uint32_t address, uint8_t *data, uint32_t length)
{
    for (; length != 0; length--)
    {
        *(volatile uint8_t *)(SRAM_BASE_ADDRESS + address) = *data++;
        address++;
    }
}

/**
 * @brief 从SRAM指定地址读取指定长度的数据
 * 
 * @param address 开始读取的地址
 * @param data 数据存储区
 * @param length 要读取的长度
 */
void SRAM_ReadData(uint32_t address, uint8_t *data, uint32_t length)
{
    for (; length != 0; length--)
    {
        *data++ = *(volatile uint8_t *)(SRAM_BASE_ADDRESS + address);
        address++;
    }
}