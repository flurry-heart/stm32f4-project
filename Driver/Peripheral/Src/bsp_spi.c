#include "bsp_spi.h"

SPI_HandleTypeDef g_spi1_handle;

/**
 * @brief SPI初始化
 * 
 * @param hspi SPI句柄
 * @param SPIx SPI基地址，可选值: SPIx, x可取: 1 ~ 3
 * @param CLKPolarity 时钟极性，可选值: [SPI_POLARITY_LOW, SPI_POLARITY_HIGH]
 * @param CLKPhase 时钟相位，可选值: [SPI_PHASE_1EDGE, SPI_PHASE_2EDGE]]
 * @param perscaler 时钟分频因子，可选值: SPI_BAUDRATEPRESCALER_x, x可取范围: [2, 4, 6, 8, 16, 32, 64, 128, 256]
 * @param SPI_FirstBit 数据有效性顺序，可选值: [SPI_FIRSTBIT_MSB, SPI_FIRSTBIT_LSB]
 */
void BSP_SPI_Init(SPI_HandleTypeDef *hspi, SPI_TypeDef *SPIx,uint32_t CLKPolarity, uint32_t CLKPhase, uint32_t perscaler,uint32_t firstBit)
{
    hspi->Instance = SPIx;                                                      // SPI基地址
    hspi->Init.Mode = SPI_MODE_MASTER;                                          // SPI主机模式
    hspi->Init.Direction = SPI_DIRECTION_2LINES;                                // SPI全双工模式
    hspi->Init.DataSize = SPI_DATASIZE_8BIT;                                    // SPI帧格式
    hspi->Init.CLKPolarity = CLKPolarity;                                       // SPI时钟极性
    hspi->Init.CLKPhase = CLKPhase;                                             // SPI时钟相位
    hspi->Init.NSS = SPI_NSS_SOFT;                                              // SPI软件NSS控制
    hspi->Init.BaudRatePrescaler = perscaler;                                   // SPI时钟分频因子
    hspi->Init.FirstBit = firstBit;                                             // SPI数据高位先发送
    hspi->Init.TIMode = SPI_TIMODE_DISABLE;                                     // SPI不使用TI模式
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;                     // SPI不使用CRC校验
    hspi->Init.CRCPolynomial = 7;                                               // 设置CRC校验多项式
    HAL_SPI_Init(hspi);
}

/**
 * @brief SPI底层初始化函数
 * 
 * @param hspi SPI句柄
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hspi->Instance == SPI1)
    {
        __HAL_RCC_SPI1_CLK_ENABLE();                                            // 使能SPI2时钟
        __HAL_RCC_GPIOB_CLK_ENABLE();                                           // 使能SPI2对应的GPIO时钟

        GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 ;            // SPI1的SCK引脚、MISO引脚和MOSI引脚
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                                 // 复用推完输出
        GPIO_InitStruct.Pull = GPIO_NOPULL;                                     // 不使用上下拉电阻
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                           // 输出速度
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;                              // 复用功能
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

/**
 * @brief SPI发送接收一个字节函数
 * 
 * @param hspi SPI句柄
 * @param data 要发送的数据
 * @return uint8_t 接收的数据
 */
uint8_t BSP_SPI_SwapOneByte(SPI_HandleTypeDef *hspi, uint8_t data)
{
    uint8_t receive = 0;

    HAL_SPI_TransmitReceive(hspi, &data, &receive, 1, 1000);

    return receive;
}

/**
 * @brief SPI发送接收多个字节函数
 * 
 * @param hspi SPI句柄
 * @param data 要发送的数据数组
 * @param length 要发送的数据数组长度
 */
void BSP_SPI_SendBytes(SPI_HandleTypeDef *hspi, uint8_t *data, uint16_t length)
{
    HAL_SPI_Transmit(hspi, data, length, 1000);
}

/**
 * @brief SPI初始化函数
 * 
 */
void BSP_Simulate_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能SPI SCK MISO MOSI对应GPIO引脚的时钟
    RCC_SPI_SCK_GPIO_CLK_ENABLE();
    RCC_SPI_MISO_GPIO_CLK_ENABLE();
    RCC_SPI_MOSI_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = SPI_SCK_GPIO_PIN;                                     // SPI的SCL引脚
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;                                 // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;                                         // 不使用上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                               // 输出速度
    HAL_GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI_MOSI_GPIO_PIN;                                    // SPI的MOSI引脚
    HAL_GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI_MISO_GPIO_PIN;                                    // SPI的MISO引脚
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;                                     // 输入模式
    HAL_GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStruct);


    SPI_SCK(0);                                                                 // SPI的SCK引脚默认为低电平，选择工作模式0或1
    // SPI_SCK(1);                                                                 // SPI的SCK引脚默认为高电平，选择工作模式2或3
}

/**
 * @brief SPI交换一个字节函数
 * 
 * @param data 待交换的数据
 * @return uint8_t 交换后的数据
 */
uint8_t BSP_Simulate_SPI_SwapOneByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        // SCK下降沿
        SPI_SCK(0);
        Delay_us(1);
        // 移出数据
        SPI_MOSI(data & 0x80);
        data <<= 1;
        // 移入数据
        if (SPI_MISO())
        {
            data |= 0x01;
        }
        // SCK上升沿
        SPI_SCK(1);
        Delay_us(1);
    }

    SPI_SCK(0);

    return data;
}