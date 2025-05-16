#include "lcd.h"

static void FSMC_LCD_Init(void);
static void LCD_GPIO_Init(void);
static void LCD_ILI9481_Init(void);

static void LCD_WriteCmd(uint16_t cmd);
static void LCD_WriteData(uint16_t data);
static uint16_t LCD_ReadData(void);

/**
 * @brief LCD初始化函数
 * 
 */
void LCD_Init(void)
{
    LCD_GPIO_Init();
    FSMC_LCD_Init();
    LCD_ILI9481_Init();

    HAL_Delay(10);

    LCD_Clear(WHITE);
}

/**
 * @brief FSMC驱动LCD函数
 * 
 */
static void FSMC_LCD_Init(void)
{
    SRAM_HandleTypeDef hsram = {0};

    FSMC_NORSRAM_TimingTypeDef WriteDataTimingStruct = {0};
    FSMC_NORSRAM_TimingTypeDef ReadDataTimingStruct = {0};

    hsram.Instance = FSMC_NORSRAM_DEVICE;
    hsram.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;

    hsram.Init.NSBank = (LCD_FSMC_NEX == 1) ? FSMC_NORSRAM_BANK1 : \
                                 (LCD_FSMC_NEX == 2) ? FSMC_NORSRAM_BANK2 : \
                                 (LCD_FSMC_NEX == 3) ? FSMC_NORSRAM_BANK3 : 
                                                        FSMC_NORSRAM_BANK4;     // 根据配置选择FSMC_NE1~4
    hsram.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;                  // 地址/数据线不复用
    hsram.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;                              // SRAM
    hsram.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;                 // 16位数据宽度
    hsram.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;                // 是使能突发访问，仅对同步突发存储器有效
    hsram.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;              // 等待信号的极性，仅在突发模式访问下有
    hsram.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;                   // 存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT
    hsram.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;                    // 存储器写使能
    hsram.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;                           // 等待使能位
    hsram.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;                        // 读写使用不同的时序
    hsram.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;               // 是否使能同步传输模式下的等待信号
    hsram.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;                           // 禁止突发写
    hsram.Init.ContinuousClock = FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;

    // FSMC写时序控制寄存器
    WriteDataTimingStruct.AccessMode = FSMC_ACCESS_MODE_A;                      // 模式A
    WriteDataTimingStruct.AddressSetupTime = 0x09;                              // 写数据地址建立时间（ADDSET），可以理解为RD/WR的高电平时间
    WriteDataTimingStruct.AddressHoldTime = 0x00;                               // 写数据地址保持时间（ADDHLD）模式A未用到
    WriteDataTimingStruct.DataSetupTime = 0x08;                                 // 写数据数据保存时间，可以理解为RD/WR的低电平时间
    WriteDataTimingStruct.BusTurnAroundDuration = 0x00;

    // FSMC读时序控制寄存器
    ReadDataTimingStruct.AccessMode = FSMC_ACCESS_MODE_A;                       // 模式A
    ReadDataTimingStruct.AddressSetupTime = 0x0F;                               // 读数据地址建立时间（ADDSET），可以理解为RD/WR的高电平时间
    ReadDataTimingStruct.AddressHoldTime = 0x00;                                // 读数据地址保持时间（ADDHLD）模式A未用到
    ReadDataTimingStruct.DataSetupTime = 60;                                    // 读数据数据保存时间，可以理解为RD/WR的低电平时间
    ReadDataTimingStruct.BusTurnAroundDuration = 0x00;  

    HAL_SRAM_Init(&hsram, &WriteDataTimingStruct, &ReadDataTimingStruct);
}

/**
 * @brief LCD底层初始化
 * 
 */
static void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_FSMC_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /** FSMC GPIO Configuration
        PF12 ------> FSMC_A6

        PG12 ------> FSMC_NE4

        PE7  ------> FSMC_D4    PE8  ------> FSMC_D5    PE9  ------> FSMC_D6
        PE10 ------> FSMC_D7    PE11 ------> FSMC_D8    PE12 ------> FSMC_D9
        PE13 ------> FSMC_D10   PE14 ------> FSMC_D11   PE15 ------> FSMC_D12
  
        PD0  ------> FSMC_D2    PD1  ------> FSMC_D3    PD8  ------> FSMC_D13
        PD9  ------> FSMC_D14   PD10 ------> FSMC_D15   PD14 ------> FSMC_D0
        PD15 ------> FSMC_D1

        PD4  ------> FSMC_NOE   PD5  ------> FSMC_NWE   
    */
    // PF12
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    // PG12
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    // PE7~PE15
    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                            GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                            GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    // PD0~PD1 PD4~PD5 PD8~PD10 PD14~PD15
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | 
                            GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | 
                            GPIO_PIN_5;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/**
 * @brief ILI9481初始化函数
 * 
 */
static void LCD_ILI9481_Init(void)
{
    LCD_WriteCmd(0xFF);
    LCD_WriteCmd(0xFF);
    HAL_Delay(5);

    LCD_WriteCmd(0xFF);
    LCD_WriteCmd(0xFF);
    LCD_WriteCmd(0xFF);
    LCD_WriteCmd(0xFF);
    HAL_Delay(10);

    LCD_WriteCmd(0xB0);
    LCD_WriteData(0x00);

    LCD_WriteCmd(0xB3);
    LCD_WriteData(0x02);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);

    LCD_WriteCmd(0xC0);
    LCD_WriteData(0x13);
    LCD_WriteData(0x3B);                                                        // 480
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x01);
    LCD_WriteData(0x00);                                                        // NW
    LCD_WriteData(0x43);

    LCD_WriteCmd(0xC1);
    LCD_WriteData(0x08);
    LCD_WriteData(0x1B);                                                        // CLOCK
    LCD_WriteData(0x08);
    LCD_WriteData(0x08);

    LCD_WriteCmd(0xC4);
    LCD_WriteData(0x11);
    LCD_WriteData(0x01);
    LCD_WriteData(0x73);
    LCD_WriteData(0x01);

    LCD_WriteCmd(0xC6);
    LCD_WriteData(0x00);

    LCD_WriteCmd(0xC8);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x05);
    LCD_WriteData(0x14);
    LCD_WriteData(0x5C);
    LCD_WriteData(0x03);
    LCD_WriteData(0x07);
    LCD_WriteData(0x07);
    LCD_WriteData(0x10);
    LCD_WriteData(0x00);
    LCD_WriteData(0x23);

    LCD_WriteData(0x10);
    LCD_WriteData(0x07);
    LCD_WriteData(0x07);
    LCD_WriteData(0x53);
    LCD_WriteData(0x0C);
    LCD_WriteData(0x14);
    LCD_WriteData(0x05);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x23);
    LCD_WriteData(0x00);

    LCD_WriteCmd(0x35);
    LCD_WriteData(0x00);

    LCD_WriteCmd(0x44);
    LCD_WriteData(0x00);
    LCD_WriteData(0x01);

    LCD_WriteCmd(0xD0);
    LCD_WriteData(0x07);
    LCD_WriteData(0x07);                                                        // VCI1
    LCD_WriteData(0x1D);                                                        // VRH
    LCD_WriteData(0x03);                                                        // BT

    LCD_WriteCmd(0xD1);
    LCD_WriteData(0x03);
    LCD_WriteData(0x5B);                                                        // VCM
    LCD_WriteData(0x10);                                                        // VDV

    LCD_WriteCmd(0xD2);
    LCD_WriteData(0x03);
    LCD_WriteData(0x24);
    LCD_WriteData(0x04);

    LCD_WriteCmd(0x2A);                                                         // 设置列地址
    LCD_WriteData(0x00);                                                        // 发送列地址的起始地址高8位
    LCD_WriteData(0x00);                                                        // 发送列地址的起始地址低8位
    LCD_WriteData(0x01);                                                        // 发送列地址的结束地址高8位
    LCD_WriteData(0x3F);                                                        // 发送列地址的结束地址低8位

    LCD_WriteCmd(0x2B);                                                         // 设置页地址
    LCD_WriteData(0x00);                                                        // 发送页地址的起始地址高8位
    LCD_WriteData(0x00);                                                        // 发送页地址的起始地址低8位
    LCD_WriteData(0x01);                                                        // 发送页地址的结束地址高8位
    LCD_WriteData(0xDF);                                                        // 发送页地址的结束地址低8位

    LCD_WriteCmd(0x36);                                                         // 访问控制，设置GRAM读写方向，控制显示方向
    LCD_WriteData(0x00);                                                        // 从左到右，从上到下，颜色为RGB顺序

    LCD_WriteCmd(0xC0);
    LCD_WriteData(0x13);

    LCD_WriteCmd(0x3A);
    LCD_WriteData(0x55);

    LCD_WriteCmd(0x11);
    HAL_Delay(150);

    LCD_WriteCmd(0x29);
    HAL_Delay(30);
}

/**
 * @brief LCD写命令函数
 * 
 * @param cmd 命令
 */
static void LCD_WriteCmd(uint16_t cmd)
{
    LCD_CMD = cmd;
}

/**
 * @brief LCD写数据函数
 * 
 * @param data 数据
 */
static void LCD_WriteData(uint16_t data)
{
    LCD_DATA = data;
}

/**
 * @brief LCD读数据函数
 * 
 * @return uint16_t 读取到RGB565值
 */
static uint16_t LCD_ReadData(void)
{
    return LCD_DATA;
}

/**
 * @brief 设置光标位置
 * 
 * @param x 列
 * @param y 行
 */
void LCD_SetCursor(uint16_t x, uint16_t y)
{
    LCD_WriteCmd(0x2A);                                                         // 设置列地址
    LCD_WriteData(x >> 8);                                                      // 发送列地址的起始地址高8位
    LCD_WriteData(x & 0xFF);                                                    // 发送列地址的起始地址低8位

    LCD_WriteCmd(0x2B);                                                         // 发送页地址
    LCD_WriteData(y >> 8);                                                      // 发送页地址的起始地址高8位
    LCD_WriteData(y & 0xFF);                                                    // 发送页地址的起始地址低8位
}

/**
 * @brief 设置LCD的光标范围
 * 
 * @param x1 光标的起始位置的列
 * @param y1 光标的起始位置的行
 * @param x2 光标的结束位置的列
 * @param y2 光标的结束位置的行
 */
void LCD_SetCursorArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_WriteCmd(0x2A);                                                         // 设置列地址
    LCD_WriteData(x1 >> 8);                                                     // 发送列地址的起始地址高8位
    LCD_WriteData(x1 & 0xFF);                                                   // 发送列地址的起始地址低8位
    LCD_WriteData(x2 >> 8);                                                     // 发送列地址的起始地址高8位
    LCD_WriteData(x2 & 0xFF);                                                   // 发送列地址的起始地址低8位

    LCD_WriteCmd(0x2B);                                                         // 发送页地址
    LCD_WriteData(y1 >> 8);                                                     // 发送页地址的起始地址高8位
    LCD_WriteData(y1 & 0xFF);                                                   // 发送页地址的起始地址低8位
    LCD_WriteData(y2 >> 8);                                                     // 发送页地址的起始地址高8位
    LCD_WriteData(y2 & 0xFF);                                                   // 发送页地址的起始地址低8位
}

/**
 * @brief LCD设置显示方向函数
 * 
 * @param direction 0：从左到右，从上到下
 *                  1：从上到下，从左到右
 *                  2：从右到左，从上到下
 *                  3：从上到下，从右到左
 *                  4：从左到右，从下到上
 *                  5：从下到上，从左到右
 *                  6：从右到左，从下到上
 *                  7：从下到上，从右到左
 */
void LCD_DisplayDirection(uint8_t mode)
{
    LCD_WriteCmd(0x36);                                                         //设置彩屏显示方向的寄存器
    LCD_WriteData(0x00 | (mode << 5));
    
    switch (mode)
    {
    case 0:
    case 2:
    case 4:
    case 6:
        LCD_SetCursorArea(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
        break;

    case 1:
    case 3:
    case 5:
    case 7:
        LCD_SetCursorArea(0, 0, LCD_HEIGHT - 1, LCD_WIDTH - 1);
    
    default:
        break;
    }
}

/**
 * @brief LCD清屏函数
 * 
 * @param color 颜色
 */
void LCD_Clear(uint16_t color)
{
    uint32_t total_point = LCD_WIDTH * LCD_HEIGHT;                              // 得到总点数
  
    LCD_SetCursor(0, 0);                                                        // 设置光标位置
    LCD_WriteCmd(0x2C);                                                         // 发送写GRAM指令

    for (uint32_t index = 0; index < total_point; index++)
    {
        LCD_WriteData(color);
    }
}

/**
 * @brief LCD局部清屏函数
 * 
 * @param x 要清空的区域的左上角的列坐标
 * @param y 要清空的区域的左上角的行坐标
 * @param width 要清空的区域的宽度
 * @param height 要清空的区域的高度
 * @param color 要清空的区域的颜色
 */
void LCD_ClearArea(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    for (uint8_t i = y; i < y + height; i++)
    {
        LCD_SetCursor(x, i);                                                    // 设置光标位置
        LCD_WriteCmd(0x2C);                                                     // 发送写GRAM指令
        for (uint8_t j = x; j < x + width; j++)
        {
            LCD_WriteData(color);
        }
    }
}

/**
 * @brief LCD画点函数
 * 
 * @param x 列
 * @param y 行
 * @param color 颜色
 */
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
  
    LCD_SetCursor(x, y);                                                        // 设置坐标
    LCD_WriteCmd(0x2C);                                                         // 发送写GRAM指令
    LCD_WriteData(color);                                                       // 写入颜色值
}

/**
 * @brief LCD读点函数
 * 
 * @param x 列数
 * @param y 行数
 * @return uint16_t 读取的RGB565的颜色值
 */
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y)
{
    uint16_t r = 0, g = 0, b = 0;

    LCD_SetCursor(x, y);                                                        // 设置坐标
    LCD_WriteCmd(0x2E);                                                         // 读GRAM数据指令
    LCD_ReadData();                                                             // 假读
    r = LCD_ReadData();                                                         // 读取R通道和G通道的值
    b = LCD_ReadData();                                                         // 读取B通道的值
    g = r & 0xFF;                                                               // 获取G通道的值
    return (((r >> 11) << 11) | ((g >> 2) << 5) | (b >> 11));
}

/**
 * @brief LCD显示字符函数
 * 
 * @param x 列
 * @param y 行
 * @param chr 显示的字符
 * @param size 字体大小，这里字符的高度等于字重，字符的宽度等于字重的一半
 * @param fColor 字符的颜色
 * @param bColor 背景色
 * @param displayMode 显示模式的枚举值
 */
void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DisplayMode displayMode)
{
    uint8_t *pfont = NULL;
    uint8_t temp = 0;
    uint8_t cHigh = size / 8 + ((size % 8) ? 1 : 0);                            // 得到一个字符对应的字节数

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)ascii_06x12[chr - ' '];                          // 调用06x12字体
            break;

        case 16:
            pfont = (uint8_t *)ascii_08x16[chr - ' '];                          // 调用08x16字体
            break;

        case 24:
            pfont = (uint8_t *)ascii_12x24[chr - ' '];                          // 调用12x24字体
            break;

        case 32:
            pfont = (uint8_t *)ascii_16x32[chr - ' '];                          // 调用16x32字体
            break;

        default:
            return ;
    }

    for (uint8_t h = 0; h < cHigh; h++)                                         // 遍历字符的高度
    {
        for (uint8_t w = 0; w < size / 2; w++)                                  // 遍历字符的宽度
        {
            temp = pfont[h * size / 2 + w];                                     // 获取字符对应的字节数据
            for (uint8_t k = 0; k < 8; k++)                                     // 一个字节8个像素点
            {
                if (temp & 0x01)                                                // 绘制字符
                {
                    LCD_DrawPoint(x + w, y + k + 8 * h , fColor);
                }
                else
                {
                    if (displayMode == LCD_DISPLAY_NORMAL)                      // 是否绘制背景
                    {
                        LCD_DrawPoint(x + w, y + k + 8 * h , bColor);
                    }
                }
                temp >>= 1;
            }
        }
    }
}

/**
 * @brief LCD显示字符串函数
 * 
 * @param x 列
 * @param y 行
 * @param str 显示的字符串
 * @param size 字体大小，这里字符的高度等于字重，字符的宽度等于字重的一半
 * @param fColor 字符串的颜色
 * @param bColor 背景色
 * @param displayMode 显示模式的枚举值
 */
void LCD_ShowString(uint16_t x, uint16_t y, char *str, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DisplayMode displayMode)
{
    uint16_t x0 = x;
    for (uint16_t i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\n')
        {
            x = x0;
            y += size;
            continue;
        }
        LCD_ShowChar(x, y, str[i], size, fColor, bColor, displayMode);
        x += (size / 2);
    }
}

/**
 * @brief LCD显示汉字函数
 * 
 * @param x 列
 * @param y 行
 * @param chinese 要显示的汉字
 * @param size 要显示的汉字大小
 * @param fColor 汉字的颜色
 * @param bColor 背景色
 * @param displayMode 显示模式的枚举值
 */
void LCD_ShowChinese(uint16_t x, uint16_t y, char *chinese, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DisplayMode displayMode)
{
    char sigleChinese[4] = {0};                                                 // 存储单个汉字
    uint16_t index = 0;                                                         // 汉字索引
    uint16_t cHigh = size / 8 + ((size % 8) ? 1 : 0);                           // 得到一个字符对应的字节数
    uint16_t temp = 0;
    uint16_t j = 0;

    switch (size)
    {
    case 32:
        for (uint16_t i = 0; chinese[i] != '\0'; i++)                           // 遍历汉字字符串
        {
            // 获取单个汉字，一般UTF-8编码使用3个字节存储汉字，GBK编码使用2个字节存储汉字
            sigleChinese[index] = chinese[i];
            index++;
            index = index % 3;

            if (index == 0)                                                     // 汉字索引为0，说明已经获取了一个汉字
            {
                for (j = 0; strcmp(chinese_32x32[j].index, "") != 0; j++)       // 遍历汉字数组
                {
                    if (strcmp(chinese_32x32[j].index, sigleChinese) == 0)      // 找到汉字
                    {
                        break;
                    } 
                }

                for (uint16_t h = 0; h < cHigh; h++)                            // 遍历字符的高度
                {
                    for (uint16_t w = 0; w < size; w++)                         // 遍历字符的宽度
                    {
                        temp = chinese_32x32[j].data[h * size  + w];            // 获取字符对应的字节数据
                        for (uint16_t k = 0; k < 8; k++)                        // 一个字节8个像素点
                        {
                            if (temp & 0x01)                                    // 绘制字体
                            {
                                // ((i + 1) / 3)定位到第几个汉字
                                LCD_DrawPoint(x + w + ((i + 1) / 3 - 1) * size, y + k + 8 * h , fColor);
                            }
                            else
                            {
                                if (displayMode == LCD_DISPLAY_NORMAL)          // 是否绘制背景
                                {
                                    LCD_DrawPoint(x + w + ((i + 1) / 3 - 1) * size, y + k + 8 * h , bColor);
                                }  
                            }
                            
                            temp >>= 1;
                        }
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}

/**
 * @brief LCD显示数字函数
 * 
 * @param x 列
 * @param y 行
 * @param num 要显示的数字
 * @param size 要显示的数字大小
 * @param fColor 要显示的数字的颜色
 * @param bColor 背景色
 * @param dataFormat 格式化结构体
 */
void LCD_ShowNumber(uint16_t x, uint16_t y, int num, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DataFormat dataFormat)
{
    char str[20] = {0};

    if (dataFormat.align_right)
    {
        if (dataFormat.symbol)
        {
            sprintf(str, "%+*d", dataFormat.digit, num);                        // 显示符号右对齐
        }
        else
        {
            sprintf(str, "%*d", dataFormat.digit, num);                         // 不显示符号右对齐
        }
    }
    else
    {
        if (dataFormat.symbol)                                  
        {
            sprintf(str, "%+d", num);                                           // 显示符号左对齐
        }
        else                                                    
        {
            sprintf(str, "%d", num);                                            // 不显示符号左对齐
        }
    }

    LCD_ShowString(x, y, str, size, fColor, bColor, dataFormat.display_mode);
}

/**
 * @brief LCD显示浮点数函数
 * 
 * @param x 列
 * @param y 行
 * @param num 要显示的数字
 * @param size 要显示的数字大小
 * @param forecolor要显示的数字的前景色
 * @param backcolor背景色
 * @param dataFormat 格式化结构体
 */
void lcd_show_decimal(uint16_t x, uint16_t y, double decimal, uint16_t size, uint16_t forecolor, uint16_t backcolor, LCD_DataFormat data_format)
{
    int num = (int)decimal;
    int temp = num;
    int len = 0;
    int x1 = x;

    data_format.decimal = data_format.decimal ? data_format.decimal : 2;        // 小数部分的位数默认为两位

    // 获取整数的位数
    while (temp)
    {
        temp /= 10;
        len++;
    }

    // 获取整数的位数
    if (data_format.digit > len)
    {
        data_format.digit -= data_format.decimal;                               // 获取整数的位数
        data_format.align_right ? data_format.digit-- : data_format.digit;      // 如果右对齐的话，还要左移一位，显示小数点
        data_format.digit = (data_format.digit > len) ? data_format.digit : len;// 确保整数的位数大于等于指定值的整数位
    }
  
    LCD_ShowNumber(x, y, num, size, forecolor, backcolor, data_format);         // 显示整数位

    // 显示小数点
    if (data_format.digit > len)                                                // 如果整数位数大于实际的整数位数
    {
        if (data_format.align_right)                                            // 如果右对齐的话
        {
            x1 += (data_format.digit * (size / 2));                             // 光标移动到整数位数后
        }
        else                                                                    // 如果是左对齐的话
        {
            x1 += (len * (size / 2));                                           // 光标移动实际的整数位后
            x1 = ((num < 0) || (data_format.symbol)) ? x1 + (size / 2) : x1;    // 如果有符号的话，光标还需要右移一位
        }
    }
    else                                                                        // 如果整数位数等于实际的整数位数
    {
        x1 += (len * (size / 2));                                               // 光标移动到实际的整数位数后
        x1 = ((num < 0) || (data_format.symbol)) ? x1 + (size / 2) : x1;        // 如果有符号的话，光标还需要右移一位
    }
  
    LCD_ShowChar(x1, y, '.', size, forecolor, backcolor, data_format.display_mode); // 显示小数点
    x1 += (size / 2);                                                           // 光标移动到小数点位置

    // 显示小数部分
    data_format.symbol = 0;                                                     // 小数部分不显示符号
    data_format.digit = data_format.decimal;                                    // 放大后，重新设置要显示的位数
    data_format.align_right = 0;                                                // 小数部分为左对齐

    // 放大小数位，并且确保得到的结果为整数
    temp = 1;
    for (int i = 0; i < data_format.decimal; i++)
    {
        temp *= 10;
    }
    num = decimal * temp - num * temp;                                          // 小数部分取整
    num = num > 0 ? num : -num;                                                 // 确保小数部分为正数

    LCD_ShowNumber(x1, y, num, size, forecolor, backcolor, data_format);        // 显示小数部分
}

/**
 * @brief LCD打印函数
 * 
 * @param x 列
 * @param y 行
 * @param size 要显示的字符串大小
 * @param fColor 字符串的景色
 * @param bColor 背景色
 * @param displayMode 显示模式的枚举值
 * @param format 指定要显示的格式化字符串
 * @param ... 格式化字符串参数列表
 * 
 * @note 此函数对浮点数的支持不好，使用浮点数时无法显示
 */
void LCD_Printf(uint16_t x, uint16_t y, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DisplayMode displayMode, char *format, ...)
{
    char str[256];                                                              // 定义字符数组
    va_list arg;                                                                // 定义可变参数列表数据类型的变量arg
    va_start(arg, format);                                                      // 从format开始，接收参数列表到arg变量
    vsprintf(str, format, arg);                                                 // 使用vsprintf打印格式化字符串和参数列表到字符数组中
    va_end(arg);                                                                // 结束变量arg
    LCD_ShowString(x, y, str, size, fColor, bColor, displayMode);               // OLED显示字符数组（字符串）
}

/**
 * @brief LCD显示图片函数
 * 
 * @param image 图片数据
 * @param x 列
 * @param y 行
 * @param width 图片的宽度
 * @param height 图片的高度
 */
void LCD_ShowPicture(uint8_t image[], uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint16_t color = 0;

    uint32_t i = 0, j = 0, k = 0;

    for (i = 0; i < height; i++)
    {
        LCD_SetCursor(x, y + i);                                                // 设置光标位置
        LCD_WriteCmd(0x2C);                                                     // 发送写GRAM指令
  
        for (j = 0; j < width; j++)
        {
            color = (image[k]) | (image[k + 1] << 8);                           // 16位颜色
            k += 2;
            LCD_WriteData(color);                                               // 写入颜色值
        }
    }
}

/**
 * @brief LCD画线函数
 * 
 * @param x1 第一个点的列
 * @param y1 第一个点的行
 * @param x2 第二个点的列
 * @param y2 第二个点的行
 * @param size 线宽
 * @param color 线的颜色
 */
void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    int16_t x = 0, y = 0, dx = 0, dy = 0, d = 0, incrE = 0, incrNE = 0, temp = 0;
    int16_t x0_t = x0, y0_t = y0, x1_t = x1, y1_t = y1;
    uint16_t yFlag = 0, xyFlag = 0;

    if (y0_t == y1_t)                                                           // 横线单独处理
    {
        if (x0_t > x1_t)                                                        // 0号点x坐标大于1号点x坐标，则交换两点x坐标
        {
            temp = x0_t; 
            x0_t = x1_t; 
            x1_t = temp;
        }
  
        for (x = x0_t; x <= x1_t; x++)                                          // 遍历x坐标，依次画点
        {
            LCD_DrawPoint(x, y0, color);
        }
    }
    else if (x0_t == x1_t)                                                      // 竖线单独处理
    {
        if (y0_t > y1_t)                                                        // 0号点y坐标大于1号点y坐标，则交换两点y坐标
        {
            temp = y0_t;
            y0_t = y1_t;
            y1_t = temp;
        }
  
        for (y = y0_t; y <= y1_t; y++)                                          // 遍历y坐标，依次画点
        {
            LCD_DrawPoint(x0, y, color);
        }
    }
    else                                                                        // 斜线
    {
        // 使用Bresenham算法画直线，可以避免耗时的浮点运算，效率更高
        if (x0_t > x1_t)                                                        // 0号点x坐标大于1号点x坐标，交换两点坐标
        {
            // 交换后不影响画线，但是画线方向由第一、二、三、四象限变为第一、四象限
            temp = x0_t; x0_t = x1_t; x1_t = temp;
            temp = y0_t; y0_t = y1_t; y1_t = temp;
        }
  
        if (y0_t > y1_t)                                                        // 0号点y坐标大于1号点y坐标，将y坐标取负
        {
            // 取负后影响画线，但是画线方向由第一、四象限变为第一象限
            y0_t = -y0_t;
            y1_t = -y1_t;
  
            yFlag = 1;                                                          // 置标志位yflag，记住当前变换，在后续实际画线时，再将坐标换回来
        }
  
        if (y1_t - y0_t > x1_t - x0_t)	                                        // 画线斜率大于1，将x坐标与y坐标互换
        {
            // 互换后影响画线，但是画线方向由第一象限0~90度范围变为第一象限0~45度范围
            temp = x0_t; x0_t = y0_t; y0_t = temp;
            temp = x1_t; x1_t = y1_t; y1_t = temp;
  
            xyFlag = 1;                                                         // 置标志位xyflag，记住当前变换，在后续实际画线时，再将坐标换回来
        }
  
        // 以下为Bresenham算法画直线，算法要求，画线方向必须为第一象限0~45度范围
        dx = x1_t - x0_t;
        dy = y1_t - y0_t;
        incrE = 2 * dy;
        incrNE = 2 * (dy - dx);
        d = 2 * dy - dx;
        x = x0_t;
        y = y0_t;

        // 画起始点，同时判断标志位，将坐标换回来
        if (yFlag && xyFlag) { 
            LCD_DrawPoint(y, -x, color); 
        } else if (yFlag) { 
            LCD_DrawPoint(x, -y, color); 
        } else if (xyFlag) { 
            LCD_DrawPoint(y, x, color); 
        } else { 
            LCD_DrawPoint(x, y, color); 
        }

        while (x < x1_t)                                                        // 遍历x轴的每个点
        {
            x++;
            if (d < 0)                                                          // 下一个点在当前点东方
            {
                d += incrE;
            }
            else                                                                // 下一个点在当前点东北方
            {
                y++;
                d += incrNE;
            }

            // 画每一个点，同时判断标志位，将坐标换回来
            if (yFlag && xyFlag) { 
                LCD_DrawPoint(y, -x, color); 
            } else if (yFlag) { 
                LCD_DrawPoint(x, -y, color); 
            } else if (xyFlag) { 
                LCD_DrawPoint(y, x, color); 
            } else { 
                LCD_DrawPoint(x, y, color); 
            }
        }
    }
}

/**
 * @brief LCD绘制矩形
 * 
 * @param x 列
 * @param y 行
 * @param width 要绘制矩形的宽
 * @param height 要绘制矩形的高
 * @param color 要绘制的颜色
 * @param isFilled 是否要填充 1: 填充，0: 不填充
 */
void LCD_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color, uint8_t isFilled)
{
    int16_t i = 0, j = 0;

    if (!isFilled)		                                                        // 指定矩形不填充
    {
        for (i = x; i < x + width; i++)                                         // 遍历上下x坐标，画矩形上下两条线
        {
            LCD_DrawPoint(i, y, color);
            LCD_DrawPoint(i, y + height - 1, color);
        }

        for (i = y; i < y + height; i++)                                        // 遍历左右y坐标，画矩形左右两条线
        {
            LCD_DrawPoint(x, i, color);
            LCD_DrawPoint(x + width - 1, i, color);
        }
    }
    else                                                                        // 指定矩形填充
    {
        for (i = x; i < x + width; i++)                                         // 遍历x坐标
        {
            for (j = y; j < y + height; j++)                                    // 遍历y坐标
            {
                LCD_DrawPoint(i, j, color);                                     // 在指定区域画点，填充满矩形
            }
        }
    }
}

/**
 * @brief LCD绘制三角形
 * 
 * @param x0 第一个点所在的列
 * @param y0 第一个点所在的行
 * @param x1 第二个点所在的列
 * @param y1 第二个点所在的行
 * @param x2 第三个点所在的列
 * @param y2 第三个点所在的行
 * @param color 要绘制的颜色
 * @param isFilled 是否要填充 1: 填充，0: 不填充
 */
void LCD_DrawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t isFilled)
{
    int16_t minX = x0, minY = y0, maxX = x0, maxY = y0;
    int16_t i = 0, j = 0;
    int vertexX[] = {x0, x1, x2};
    int vertexY[] = {y0, y1, y2};

    if (!isFilled)                                                              // 指定三角形不填充
    {
        // 调用画线函数，将三个点用直线连接
        LCD_DrawLine(x0, y0, x1, y1, color);
        LCD_DrawLine(x0, y0, x2, y2, color);
        LCD_DrawLine(x1, y1, x2, y2, color);
    }
    else                                                                        // 指定三角形填充
    {
        // 找到三个点最小的x、y坐标
        if (x1 < minX) {minX = x1;}
        if (x2 < minX) {minX = x2;}
        if (y1 < minY) {minY = y1;}
        if (y2 < minY) {minY = y2;}
  
        // 找到三个点最大的x、y坐标
        if (x1 > maxX) {maxX = x1;}
        if (x2 > maxX) {maxX = x2;}
        if (y1 > maxY) {maxY = y1;}
        if (y2 > maxY) {maxY = y2;}
  
  
        // 最小最大坐标之间的矩形为可能需要填充的区域
        for (i = minX; i <= maxX; i++)
        {
            for (j = minY; j <= maxY; j++)
            {
                // 调用pnpoly算法，判断指定点是否在指定三角形之中，如果在，则画点，如果不在，则不做处理
                if (pnpoly(3, vertexX, vertexY, i, j)) {LCD_DrawPoint(i, j, color);}
            }
        }
    }
}

/**
 * @brief LCD绘制圆
 * 
 * @param x 圆心所在的列
 * @param y 圆心所在的行
 * @param radius 半径
 * @param color 颜色
 * @param isFilled 是否要填充 1: 填充，0: 不填充
 */
void LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color, uint8_t isFilled)
{
    // 使用Bresenham算法画圆，可以避免耗时的浮点运算，效率更高

    int16_t x_t = 0, y_t = radius;
    int16_t d = 1 - radius;
    int16_t j = 0;

    // 画每个八分之一圆弧的起始点
    LCD_DrawPoint(x + x_t, y + y_t, color);
    LCD_DrawPoint(x - x_t, y - y_t, color);
    LCD_DrawPoint(x + y_t, y + x_t, color);
    LCD_DrawPoint(x - y_t, y - x_t, color);

    if (isFilled)		                                                        // 指定圆填充
    {
        // 遍历起始点y坐标，在指定区域画点，填充部分圆
        for (j = -y_t; j < y_t; j++)
        {
            LCD_DrawPoint(x, y + j, color);
        }
    }

    while (x_t < y_t)		                                                    // 遍历x轴的每个点
    {
        x_t++;
        if (d < 0)		                                                        // 下一个点在当前点东方
        {
            d += 2 * x_t + 1;
        }
        else			                                                        // 下一个点在当前点东南方
        {
            y_t--;
            d += 2 * (x_t - y_t) + 1;
        }
  
        // 画每个八分之一圆弧的点
        LCD_DrawPoint(x + x_t, y + y_t, color);
        LCD_DrawPoint(x + y_t, y + x_t, color);
        LCD_DrawPoint(x - x_t, y - y_t, color);
        LCD_DrawPoint(x - y_t, y - x_t, color);
        LCD_DrawPoint(x + x_t, y - y_t, color);
        LCD_DrawPoint(x + y_t, y - x_t, color);
        LCD_DrawPoint(x - x_t, y + y_t, color);
        LCD_DrawPoint(x - y_t, y + x_t, color);
  
        if (isFilled)	                                                        // 指定圆填充
        {
            // 遍历中间部分，在指定区域画点，填充部分圆
            for (j = -y_t; j < y_t; j++)
            {
                LCD_DrawPoint(x + x_t, y + j, color);
                LCD_DrawPoint(x - x_t, y + j, color);
            }
  
            // 遍历两侧部分
            for (j = -x_t; j < x_t; j++)
            {
                // 在指定区域画点，填充部分圆
                LCD_DrawPoint(x - y_t, y + j, color);
                LCD_DrawPoint(x + y_t, y + j, color);
            }
        }
    }
}

/**
 * @brief OLED绘制椭圆函数
 * 
 * @param x 圆心所在的列
 * @param y 圆心所在的行
 * @param a 椭圆的横向半轴长度
 * @param b 椭圆的纵向半轴长度
 * @param isFilled 是否要填充 1: 填充，0: 不填充
 */
void LCD_DrawEllipse(uint16_t x, uint16_t y, uint16_t a, uint16_t b, uint16_t color, uint8_t isFilled)
{
    int16_t x_t = 0;
    int16_t y_t = b;
    int16_t a_t = a, b_t = b;
    int16_t j = 0;
    float  d1 = b_t * b_t + a_t * a_t * (-b_t + 0.5);
    float d2 = 0;

    // 使用Bresenham算法画椭圆，可以避免部分耗时的浮点运算，效率更高
    if (isFilled)	//指定椭圆填充
    {
        // 遍历起始点y坐标在指定区域画点，填充部分椭圆
        for (j = -y_t; j < y_t; j++)
        {
            LCD_DrawPoint(x, y + j, color);
            LCD_DrawPoint(x, y + j, color);
        }
    }

    // 画椭圆弧的起始点
    LCD_DrawPoint(x + x_t, y + y_t, color);
    LCD_DrawPoint(x - x_t, y - y_t, color);
    LCD_DrawPoint(x - x_t, y + y_t, color);
    LCD_DrawPoint(x + x_t, y - y_t, color);

    // 画椭圆中间部分
    while (b_t * b_t * (x_t + 1) < a_t * a_t * (y_t - 0.5))
    {
        if (d1 <= 0)                                                            // 下一个点在当前点东方
        {
            d1 += b_t * b_t * (2 * x_t + 3);
        }
        else                                                                    // 下一个点在当前点东南方
        {
            d1 += b_t * b_t * (2 * x_t + 3) + a_t * a_t * (-2 * y_t + 2);
            y_t--;
        }
        x_t++;
  
        if (isFilled)                                                           // 指定椭圆填充
        {
            // 遍历中间部分
            for (j = -y_t; j < y_t; j++)
            {
                // 在指定区域画点，填充部分椭圆
                LCD_DrawPoint(x + x_t, y + j, color);
                LCD_DrawPoint(x - x_t, y + j, color);
            }
        }
  
        // 画椭圆中间部分圆弧
        LCD_DrawPoint(x + x_t, y + y_t, color);
        LCD_DrawPoint(x - x_t, y - y_t, color);
        LCD_DrawPoint(x - x_t, y + y_t, color);
        LCD_DrawPoint(x + x_t, y - y_t, color);
    }

    // 画椭圆两侧部分
    d2 = b_t * b_t * (x_t + 0.5) * (x_t + 0.5) + a_t * a_t * (y_t - 1) * (y_t - 1) - a_t * a_t * b_t * b_t;

    while (y_t > 0)
    {
        if (d2 <= 0)		                                                    // 下一个点在当前点东方
        {
            d2 += b_t * b_t * (2 * x_t + 2) + a_t * a_t * (-2 * y_t + 3);
            x_t++;
        }
        else				                                                    // 下一个点在当前点东南方
        {
            d2 += a_t * a_t * (-2 * y_t + 3);
        }
        y_t--;
  
        if (isFilled)	                                                        // 指定椭圆填充
        {
            // 遍历两侧部分，在指定区域画点，填充部分椭圆
            for (j = -y_t; j < y_t; j++)
            {
                LCD_DrawPoint(x + x_t, y + j, color);
                LCD_DrawPoint(x - x_t, y + j, color);
            }
        }
  
        /*画椭圆两侧部分圆弧*/
        LCD_DrawPoint(x + x_t, y + y_t, color);
        LCD_DrawPoint(x - x_t, y - y_t, color);
        LCD_DrawPoint(x - x_t, y + y_t, color);
        LCD_DrawPoint(x + x_t, y - y_t, color);
    }
}