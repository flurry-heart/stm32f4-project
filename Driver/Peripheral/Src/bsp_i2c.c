#include "bsp_i2c.h"

I2C_HandleTypeDef g_i2c1_handle;

/**
 * @brief I2C初始化函数
 * 
 * @param hi2c I2C句柄
 * @param I2Cx I2C寄存器基地址
 * @param speed I2C的速度
 */
void BSP_I2C_Init(I2C_HandleTypeDef *hi2c, I2C_TypeDef *I2Cx, uint32_t speed)
{
    hi2c->Instance = I2Cx;                                                      // 使用的I2C
    hi2c->Init.ClockSpeed = speed;                                              // SCL时钟频率
    hi2c->Init.DutyCycle= I2C_DUTYCYCLE_2;                                      // 时钟占空比
    hi2c->Init.OwnAddress1 = 0;                                                 // STM32自身设备地址1
    hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;                        // 地址模式
    hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;                       // 双地址模式
    hi2c->Init.OwnAddress2 = 0;                                                 // STM32自身设备地址2
    hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;                       // 通用广播地址
    hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;                           // 禁止时钟延长模式

    HAL_I2C_Init(hi2c);
}

/**
 * @brief I2C底层初始化函数
 * 
 * @param hi2c I2C句柄
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct ={0};

    if(hi2c->Instance == I2C1)
    {
        __HAL_RCC_I2C1_CLK_ENABLE();                                            // 使能I2C1时钟
        __HAL_RCC_GPIOB_CLK_ENABLE();                                           // 使能I2C1对应的GPIO时钟

        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;                          // SCL引脚和SDA引脚
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;                                 // 复用开漏输出
        GPIO_InitStruct.Pull = GPIO_PULLUP;                                     // 使用上拉电阻
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                           // 输出速度
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;                              // 复用功能

        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

/**
 * @brief 模拟I2C初始化函数
 * 
 */
void BSP_Simulate_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能I2C的SCL和SDA引脚对应的GPIO时钟
    I2C_SCL_GPIO_CLK_ENABLE();
    I2C_SDA_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = I2C_SCL_GPIO_PIN;                                     // I2C SCL引脚
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;                                 // 开漏输出模式
    GPIO_InitStruct.Pull = GPIO_PULLUP;                                         // 使用上拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                               // 输出速度
    HAL_GPIO_Init(I2C_SCL_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = I2C_SDA_GPIO_PIN;                                     // I2C SCL引脚
    HAL_GPIO_Init(I2C_SDA_GPIO_PORT, &GPIO_InitStruct);

    // 空闲时，I2C总线SCL为高电平，I2C SDA为高电平
    I2C_SCL(1);
    I2C_SDA(1);
}

/**
 * @brief I2C产生起始信号函数
 * 
 * @note SCL为高电平期间，SDA从高电平往低电平跳变
 */
void BSP_Simulate_I2C_Start(void)
{
    // 1、释放SDA和SCL，并延迟，空闲状态
    I2C_SDA(1);
    I2C_SCL(1); 
    I2C_DELAY();
    // 2、拉低SDA，SDA产生下降沿，并延迟
    I2C_SDA(0);
    I2C_DELAY();
    // 3、钳住SCL总线，准备发送数据/接收数据，并延时
    I2C_SCL(0);
    I2C_DELAY();
}

/**
 * @brief I2C产生结束信号函数
 * 
 * @note SCL为高电平期间，SDA从低电平往高电平跳变
 */
void BSP_Simulate_I2C_Stop(void)
{
    // 1、SDA拉低，SCL拉高，并延迟
    I2C_SDA(0);
    I2C_SCL(1);
    I2C_DELAY();
    // 2、拉高SDA，产生上升沿，并延迟
    I2C_SDA(1);
    I2C_DELAY();
}

/**
 * @brief 主机检测应答信号
 * 
 * @return uint8_t 0，接收应答成功；1，接收应答失败
 */
uint8_t BSP_Simulate_I2C_WaitAck(void)
{
    uint8_t waitTime = 0;
    uint8_t ack = 0;

    // 1、主机释放SDA数据线并延迟，此时外部器件可以拉低SDA线
    I2C_SDA(1);
    I2C_DELAY();
    // 2、主机拉高SCL，此时从机可以返回ACK
    I2C_SCL(1);
    I2C_DELAY();

    // 3、SCL高电平期间主机读取SDA状态，等待应答
    while (I2C_READ_SDA())                    
    {
        // 4、如果超时的话，就直接产生结束信号，非应答
        waitTime++;
        if (waitTime > 250)
        {
            BSP_Simulate_I2C_Stop();
            ack = 1;
            break;
        }
    }
    // 5、SCL=0，结束ACK检查
    I2C_SCL(0);
    I2C_DELAY();

    // 6、返回是否接收到应答信号
    return ack;
}

/**
 * @brief 发送应答信号或非应答信号
 * 
 * @param ack 0，发送应答信号；1，发送非应答信号
 */
void BSP_Simulate_I2C_SendAck(uint8_t ack)
{
    // 1、拉低SDA，表示应答，拉高SDA，表示非应答，并延迟
    I2C_SDA(ack);
    I2C_DELAY();
    // 2、主机拉高SCL线，并延迟，确保从机能有足够时间去接收SDA线上的电平信号
    I2C_SCL(1);
    I2C_DELAY();
    // 3、主机拉低时钟线并延时，完成这一位数据的传送
    I2C_SCL(0);
    I2C_DELAY();
    // 4、释放SDA线，并延迟
    I2C_SDA(1);
    I2C_DELAY();
}

/**
 * @brief I2C读取一个字节函数
 * 
 * @param ack 0，发送应答信号，1，发送非应答信号
 * @return uint8_t 
 */
uint8_t BSP_Simulate_I2C_ReadOneByte(uint8_t ack)
{
    uint8_t receive = 0;

    // 1、主机释放SDA
    I2C_SDA(1);

    for (uint8_t i = 0; i < 8; i++)
    {
        // 2、释放SCL，主机将在SCL高电平期间读取数据位
        I2C_SCL(1);
        I2C_DELAY();
        // 3、读取SDA
        if (I2C_READ_SDA())
        {
            receive |= 0x80 >> i;
        }
        // 4、拉低SCL，从机切换SDA线输出数据
        I2C_SCL(0);
        I2C_DELAY();
    }

    // 5、发送应答信号或非应答信号
    BSP_Simulate_I2C_SendAck(ack);

    // 6、返回读取的数据
    return receive;
}

/**
 * @brief I2C发送一个字节函数
 * 
 * @param data 待发送的数据
 */
void BSP_Simulate_I2C_SendOneByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        // 1、发送数据位的高位
        I2C_SDA((data & 0x80) >> 7);
        I2C_DELAY();
        // 2、释放SCL，从机将在SCL高电平期间读取数据位
        I2C_SCL(1);
        I2C_DELAY();
        // 3、拉低SCL
        I2C_SCL(0);
        // 4、数据左移一位，用于下次发送
        data <<= 1;
    }
    // 5、发送完成，主机释放SDA线
    I2C_SDA(1);
}