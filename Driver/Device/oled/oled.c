#include "oled.h"

static uint8_t g_oled_gram[8][128];                                             // GRAM缓冲区

SPI_HandleTypeDef *pg_oled_spi_handle;

static void OLED_GPIO_Init(void);
static void OLED_SSD1306_Init(void);
static void OLED_WriteOneByte(uint8_t data, OLED_WriteMode mode);
static void OLED_WriteBytes(uint8_t *data, uint16_t length, OLED_WriteMode mode);

/**
 * @brief OLED初始化函数
 * 
 * @param hspi SPI句柄
 */
void OLED_Init(SPI_HandleTypeDef *hspi)
{
    pg_oled_spi_handle = hspi;

    OLED_GPIO_Init();
    
    OLED_CS(1);
    OLED_DC(1);
    
    OLED_RESET(0);
    HAL_Delay(10);
    OLED_RESET(1);
    
    OLED_SSD1306_Init();

    OLED_Clear();
}

/**
 * @brief OLED的GPIO引脚初始化函数
 * 
 */
void OLED_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_OLED_CS_GPIO_CLK_ENABLE();
    RCC_OLED_DC_GPIO_CLK_ENABLE();
    RCC_OLED_RESET_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = OLED_CS_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_CS_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = OLED_DC_GPIO_PIN;
    HAL_GPIO_Init(OLED_DC_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = OLED_RESET_GPIO_PIN;
    HAL_GPIO_Init(OLED_RESET_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief SSD1306初始化函数
 * 
 */
void OLED_SSD1306_Init(void)
{
    OLED_WriteOneByte(0xAE, OLED_MODE_CMD);                                     // 关闭显示

    OLED_WriteOneByte(0xD5, OLED_MODE_CMD);                                     // 设置显示时钟分频比/振荡器频率
    OLED_WriteOneByte(0x80, OLED_MODE_CMD);      

    OLED_WriteOneByte(0xA8, OLED_MODE_CMD);                                     // 设置多路复用率
    OLED_WriteOneByte(0x3F, OLED_MODE_CMD);      

    OLED_WriteOneByte(0xD3, OLED_MODE_CMD);                                     // 设置显示偏移
    OLED_WriteOneByte(0x00, OLED_MODE_CMD);      

    OLED_WriteOneByte(0x40, OLED_MODE_CMD);                                     // 设置显示开始行

    OLED_WriteOneByte(0xA1, OLED_MODE_CMD);                                     // 设置左右方向，0xA1正常 0xA0左右反置
    OLED_WriteOneByte(0xC8, OLED_MODE_CMD);                                     // 设置上下方向，0xC8正常 0xC0上下反置

    OLED_WriteOneByte(0xDA, OLED_MODE_CMD);                                     // 设置COM引脚硬件配置
    OLED_WriteOneByte(0x12, OLED_MODE_CMD);      

    OLED_WriteOneByte(0x81, OLED_MODE_CMD);                                     // 设置对比度控制
    OLED_WriteOneByte(0xCF, OLED_MODE_CMD);      

    OLED_WriteOneByte(0xD9, OLED_MODE_CMD);                                     // 设置预充电周期
    OLED_WriteOneByte(0xF1, OLED_MODE_CMD);

    OLED_WriteOneByte(0xDB, OLED_MODE_CMD);                                     // 设置VCOMH取消选择级别
    OLED_WriteOneByte(0x30, OLED_MODE_CMD);

    OLED_WriteOneByte(0xA4, OLED_MODE_CMD);                                     // 设置整个显示打开/关闭
    OLED_WriteOneByte(0xA6, OLED_MODE_CMD);                                     // 设置正常/倒转显示

    OLED_WriteOneByte(0x8D, OLED_MODE_CMD);                                     // 设置充电泵
    OLED_WriteOneByte(0x14, OLED_MODE_CMD);

    OLED_WriteOneByte(0xAF, OLED_MODE_CMD);                                     // 开启显示
}

/**
 * @brief OELD往寄存器写入一个字节函数
 * 
 * @param data 一字节数据
 * @param mode OLED状态的枚举值
 */
static void OLED_WriteOneByte(uint8_t data, OLED_WriteMode mode)
{
    OLED_CS(0);                                                                 // 片选
    OLED_DC(mode);                                                              // 数据/命令选择
    BSP_SPI_SwapOneByte(pg_oled_spi_handle, data);                              // 发送数据
    OLED_CS(1);                                                                 // 取消片选
}

/**
 * @brief OELD往寄存器写入多个字节函数
 * 
 * @param data 字节数组
 * @param length 字节数组长度
 * @param mode OLED状态的枚举值
 */
static void OLED_WriteBytes(uint8_t *data, uint16_t length, OLED_WriteMode mode)
{
    OLED_CS(0);                                                                 // 片选
    OLED_DC(mode);                                                              // 数据/命令选择
    BSP_SPI_SendBytes(pg_oled_spi_handle, data, length);                        // 发送数据
    OLED_CS(1);                                                                 // 取消片选
}

/**
 * @brief OLED刷新GRAM函数
 * 
 */
void OLED_RefreshGRAM(void)
{
    for (int page = 0; page < 8; page++)
    {
        OLED_SetCursor(0, page);
        OLED_WriteBytes(g_oled_gram[page], 128, OLED_MODE_DATA);
    }
}

/**
 * @brief 开启OLED
 * 
 */
void OLED_DisplayOn(void)
{
    OLED_WriteOneByte(0x8D, OLED_MODE_CMD);                                     // SET DCDC命令
    OLED_WriteOneByte(0x14, OLED_MODE_CMD);                                     // DCDC ON
    OLED_WriteOneByte(0xAF, OLED_MODE_CMD);                                     // DISPLAY ON
}

/**
 * @brief 关闭OLED
 * 
 */
void OLED_DisplayOff(void)
{
    OLED_WriteOneByte(0X8D, OLED_MODE_CMD);                                     // SET DCDC命令
    OLED_WriteOneByte(0X10, OLED_MODE_CMD);                                     // DCDC OFF
    OLED_WriteOneByte(0XAE, OLED_MODE_CMD);                                     // DISPLAY OFF
}

/**
 * @brief OLED设置坐标函数
 * 
 * @param column 坐标所在的列，范围: 0 ~ 127
 * @param page 坐标所在的页，范围: 0 ~ 7
 */
void OLED_SetCursor(uint8_t column, uint8_t page)
{
    // 页地址、列低地址、列高地址
    uint8_t data[3] = {0xB0 | page, 0x10 | ((column & 0xF0) >> 4), 0x0F & column};

    if (column > 127 || page > 8)                                               // 超出范围
    {
        return;
    }

    OLED_WriteBytes(data, 3, OLED_MODE_CMD);
}

/**
 * @brief OLED清屏函数
 * 
 */
void OLED_Clear(void)
{
    for (int page = 0; page < 8; page++)
    {
        for (int column = 0; column < 128; column++)
        {
            g_oled_gram[page][column] = 0x00;
        }
    }
    OLED_RefreshGRAM();                                                         // 更新显示
}

/**
 * @brief OLED局部清屏函数
 * 
 * @param x 要清空的区域的左上角的列坐标
 * @param y 要清空的区域的左上角的行坐标
 * @param width 要清空的区域的宽度
 * @param height 要清空的区域的高度
 */
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    for (uint8_t i = y; i < y + height; i++)
    {
        for (uint8_t j = x; j < x + width; j++)
        {
            g_oled_gram[i >> 3][j] &= ~(0x01 << (i & 0x07));
        }
    }
}

/**
 * @brief OLED画点函数
 * 
 * @param x 要画的点的x坐标，范围: 0 ~ 127
 * @param y 要画的点的y坐标，范围: 0 ~ 63
 */
void OLED_DrawPoint(uint8_t x, uint8_t y)
{
    if (x > 127 || y > 63)                                                      // 超出范围
    {
        return;
    }

    g_oled_gram[y >> 3][x] |= 0x01 << (y & 0x07);                               // 画点
}

/**
 * @brief OLED在任意位置绘制图像
 * 
 * @param x 要绘制图像所在的列，范围: 0 ~ 127
 * @param y 要绘制图像所在的行，范围: 0 ~ 63
 * @param width 要绘制图像的宽
 * @param height 要绘制图像的高
 * @param image 要绘制图像的数据数组
 */
void OLED_ShowImage(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t * image)
{
    if (x > 127 || y > 63)                                                      // 超出范围
    {
        return;
    }

    // 清除区域后覆盖显示，如果不清空指定区域为叠加显示
    OLED_ClearArea(x, y, width, height);

    // ((height - 1) >> 3) + 1 相当于 (height - 1) / 8 + 1，即 height / 8 向上取整
    for (uint8_t row = 0; row < ((height - 1) >> 3) + 1; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // y << 3 相当于 y / 8，得到对应的page。y & 0x07 相当于 y % 8，得到一页的对应bit位
            g_oled_gram[(y >> 3) + row][x + col] |= image[row * width + col] << (y & 0x07);
            g_oled_gram[(y >> 3) + row + 1][x + col] |= image[row * width + col] >> (8 - (y & 0x07));
        }
    }
}

/**
 * @brief OLED显示字符函数
 * 
 * @param x 要显示的字符所在的列，范围: 0 ~ 127
 * @param y 要显示的字符所在的页，范围: 0 ~ 63
 * @param chr 要显示的字符
 * @param size 要显示的字符大小
 */
void OLED_ShowChar(uint8_t x, uint8_t y, char chr, uint8_t size)
{
    uint8_t *pfont = NULL;

    if (x > 127 || y > 63)                                                      // 超出范围
    {
        return;
    }

    switch (size)
    {
    case 12:
        pfont = (uint8_t *)ascii_06x12[chr - ' '];
        break;

    case 16:
        pfont = (uint8_t *)ascii_08x16[chr - ' '];
        break;
        
    default:
        return;
    }

    OLED_ShowImage(x, y, size / 2, size, pfont);
}

/**
 * @brief OLED显示字符串函数
 * 
 * @param x 要显示的字符串所在的列，范围: 0 ~ 127
 * @param y 要显示的字符串所在的页，范围: 0 ~ 63
 * @param str 要显示的字符串
 * @param size 要显示的字符串的大小
 */
void OLED_ShowString(uint8_t x, uint8_t y, char * str, uint8_t size)
{
    if (x > 127 || y > 63)                                                      // 超出范围
    {
        return;
    }

    for (uint8_t i = 0; str[i] != '\0'; i++)
    {
        OLED_ShowChar(x + i * size / 2, y, str[i], size);
    }
}

/**
 * @brief OLED显示汉字函数
 * 
 * @param x 要显示的汉字所在的列，范围: 0 ~ 127
 * @param y 要显示的汉字所在的页，范围: 0 ~ 63
 * @param chinese 要显示的汉字
 * @param size 要显示的汉字大小
 */
void OLED_ShowChinese(uint8_t x, uint8_t y, char *chinese, uint8_t size)
{
    char sigleChinese[4] = {0};                                                 // 存储单个汉字
    uint8_t index = 0;                                                          // 汉字索引
    uint8_t j = 0;

    if (x > 127 || y > 63)                                                      // 超出范围
    {
        return;
    }

    switch (size)
    {
    case 12:
        for (uint8_t i = 0; chinese[i] != '\0'; i++)                            // 遍历汉字字符串
        {
            // 获取单个汉字，一般UTF-8编码使用3个字节存储汉字，GBK编码使用2个字节存储汉字
            sigleChinese[index] = chinese[i];
            index++;
            index = index % 3;

            if (index == 0)                                                     // 汉字索引为0，说明已经获取了一个汉字
            {
                for (j = 0; strcmp(chinese_12x12[j].index, "") != 0; j++)       // 遍历汉字数组
                {
                    if (strcmp(chinese_12x12[j].index, sigleChinese) == 0)      // 找到汉字
                    {
                        break;
                    } 
                }
                // ((i + 1) / 3)定位到第几个汉字
                OLED_ShowImage(x + ((i + 1) / 3 - 1) * size, y, size, size, chinese_12x12[j].data);
            }
        }
    break;

    case 16:
        for (uint8_t i = 0; chinese[i] != '\0'; i++)                            // 遍历汉字字符串
        {
            // 获取单个汉字，一般UTF-8编码使用3个字节存储汉字，GBK编码使用2个字节存储汉字
            sigleChinese[index] = chinese[i];
            index++;
            index = index % 3;

            if (index == 0)                                                     // 汉字索引为0，说明已经获取了一个汉字
            {
                for (j = 0; strcmp(chinese_16x16[j].index, "") != 0; j++)       // 遍历汉字数组
                {
                    if (strcmp(chinese_16x16[j].index, sigleChinese) == 0)      // 找到汉字
                    {
                        break;
                    } 
                }

                OLED_ShowImage(x + ((i + 1) / 3 - 1) * size, y, size, size, chinese_16x16[j].data);
            }
        }
    break;

    default:
        return;
    }
}

/**
 * @brief OLED显示数字函数
 * 
 * @param x 要显示的数字的列，范围: 0 ~ 127
 * @param y 要显示的数字的行，范围: 0 ~ 63
 * @param num 要显示的数字
 * @param size 要显示的数字大小
 * @param dataFormat 格式化结构体
 */
void OLED_ShowNumber(uint8_t x, uint8_t y, int num, uint8_t size, DataFormat dataFormat)
{
    char str[20] = {0};

    if (x < 0 || x > 127 || y < 0 || y > 63)                                    // 超出范围
    {
        return;
    }

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

    OLED_ShowString(x, y, str, size);
}

/**
 * @brief OLED显示浮点数函数
 * 
 * @param x 要显示的数字的列，范围: 0 ~ 127
 * @param y 要显示的数字的行，范围: 0 ~ 63
 * @param num 要显示的数字
 * @param size 要显示的数字大小
 * @param dataFormat 格式化结构体
 */
void OLED_ShowDecimal(uint8_t x, uint8_t y, double decimal, uint8_t size, DataFormat dataFormat)
{
    int num = (int)decimal;
    int temp = num;
    int len = 0;
    int x1 = x;

    if (x < 0 || x > 127 || y < 0 || y > 63)                                    // 超出范围
    {
        return;
    }

    dataFormat.decimal = dataFormat.decimal ? dataFormat.decimal : 2;           // 小数部分的位数默认为两位

    // 获取整数的位数
    while (temp)
    {
        temp /= 10;
        len++;
    }
  

    // 获取整数的位数
    if (dataFormat.digit > len)
    {
        dataFormat.digit -= dataFormat.decimal;                                 // 获取整数的位数
        dataFormat.align_right ? dataFormat.digit-- : dataFormat.digit;         // 如果右对齐的话，还要左移一位，显示小数点
        dataFormat.digit = (dataFormat.digit > len) ? dataFormat.digit : len;   // 确保整数的位数大于等于指定值的整数位
    }
  
    OLED_ShowNumber(x, y, num, size, dataFormat);                               // 显示整数位

    // 显示小数点
    if (dataFormat.digit > len)                                                 // 如果整数位数大于实际的整数位数
    {
        if (dataFormat.align_right)                                             // 如果右对齐的话
        {
            x1 += (dataFormat.digit * (size / 2));                              // 光标移动到整数位数后
        }
        else                                                                    // 如果是左对齐的话
        {
            x1 += (len * (size / 2));                                           // 光标移动实际的整数位后
            x1 = ((num < 0) || (dataFormat.symbol)) ? x1 + (size / 2) : x1;     // 如果有符号的话，光标还需要右移一位
        }
    }
    else                                                                        // 如果整数位数等于实际的整数位数
    {
        x1 += (len * (size / 2));                                               // 光标移动到实际的整数位数后
        x1 = ((num < 0) || (dataFormat.symbol)) ? x1 + (size / 2) : x1;         // 如果有符号的话，光标还需要右移一位
    }
  
    OLED_ShowChar(x1, y, '.', size);                                            // 显示小数点
    x1 += (size / 2);                                                           // 光标移动到小数点位置

    // 显示小数部分
    dataFormat.symbol = 0;                                                      // 小数部分不显示符号
    dataFormat.digit = dataFormat.decimal;                                      // 放大后，重新设置要显示的位数
    dataFormat.align_right = 0;                                                 // 小数部分为左对齐

    // 放大小数位，并且确保得到的结果为整数
    temp = 1;
    for (int i = 0; i < dataFormat.decimal; i++)
    {
        temp *= 10;
    }
    num = decimal * temp - num * temp;                                          // 小数部分取整
    num = num > 0 ? num : -num;                                                 // 确保小数部分为正数

    OLED_ShowNumber(x1, y, num, size, dataFormat);                              // 显示小数部分
}

/**
 * @brief OLED打印函数
 * 
 * @param x 要显示的字符串的列，范围: 0 ~ 127
 * @param y 要显示的字符串的行，范围: 0 ~ 63
 * @param size 要显示的字符串大小
 * @param format 指定要显示的格式化字符串
 * @param ... 格式化字符串参数列表
 * 
 * @note 此函数对浮点数的支持不好，使用浮点数时无法显示
 */
void OLED_Printf(uint8_t x, uint8_t y, uint8_t size, char *format, ...)
{
    char str[256];                                                              // 定义字符数组
    va_list arg;                                                                // 定义可变参数列表数据类型的变量arg
    va_start(arg, format);                                                      // 从format开始，接收参数列表到arg变量
    vsprintf(str, format, arg);                                                 // 使用vsprintf打印格式化字符串和参数列表到字符数组中
    va_end(arg);                                                                // 结束变量arg
    OLED_ShowString(x, y, str, size);                                           // OLED显示字符数组（字符串）
}

/**
 * @brief OLED绘制直线函数
 * 
 * @param x0 起始点所在的列，范围: 0 ~ 127
 * @param y0 起始点所在的行，范围: 0 ~ 63
 * @param x1 终点所在的列，范围: 0 ~ 127
 * @param y1 终点所在的行，范围: 0 ~ 63
 */
void OLED_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    int16_t x = 0, y = 0, dx = 0, dy = 0, d = 0, incrE = 0, incrNE = 0, temp = 0;
    int16_t x0_t = x0, y0_t = y0, x1_t = x1, y1_t = y1;
    uint8_t yFlag = 0, xyFlag = 0;

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
            OLED_DrawPoint(x, y0);
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
            OLED_DrawPoint(x0, y);
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
            OLED_DrawPoint(y, -x); 
        } else if (yFlag) { 
            OLED_DrawPoint(x, -y); 
        } else if (xyFlag) { 
            OLED_DrawPoint(y, x); 
        } else { 
            OLED_DrawPoint(x, y); 
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
                OLED_DrawPoint(y, -x); 
            } else if (yFlag) { 
                OLED_DrawPoint(x, -y); 
            } else if (xyFlag) { 
                OLED_DrawPoint(y, x); 
            } else { 
                OLED_DrawPoint(x, y); 
            }
        }
    }
}

/**
 * @brief OLED绘制矩形
 * 
 * @param x 要绘制矩形所在的列，范围: 0 ~ 127
 * @param y 要绘制矩形所在的行，范围: 0 ~ 63
 * @param width 要绘制矩形的宽，范围: 1 ~ 127
 * @param height 要绘制矩形的高，范围: 1 ~ 63
 * @param isFilled 是否要填充 1: 填充，0: 不填充
 */
void OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t isFilled)
{
    uint8_t i = 0, j = 0;

    if (!isFilled)		                                                        // 指定矩形不填充
    {
        for (i = x; i < x + width; i++)                                         // 遍历上下x坐标，画矩形上下两条线
        {
            OLED_DrawPoint(i, y);
            OLED_DrawPoint(i, y + height - 1);
        }

        for (i = y; i < y + height; i++)                                        // 遍历左右y坐标，画矩形左右两条线
        {
            OLED_DrawPoint(x, i);
            OLED_DrawPoint(x + width - 1, i);
        }
    }
    else                                                                        // 指定矩形填充
    {
        for (i = x; i < x + width; i++)                                         // 遍历x坐标
        {
            for (j = y; j < y + height; j++)                                    // 遍历y坐标
            {
                OLED_DrawPoint(i, j);                                           // 在指定区域画点，填充满矩形
            }
        }
    }
}

/**
 * @brief LCD绘制三角形
 * 
 * @param x0 第一个点所在的列，范围: 0 ~ 127
 * @param y0 第一个点所在的行，范围: 0 ~ 63
 * @param x1 第二个点所在的列，范围: 0 ~ 127
 * @param y1 第二个点所在的行，范围: 0 ~ 63
 * @param x2 第三个点所在的列，范围: 0 ~ 127
 * @param y2 第三个点所在的行，范围: 0 ~ 63
 * @param isFilled 是否要填充 1: 填充，0: 不填充
 */
void OLED_DrawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t isFilled)
{
    int16_t minX = x0, minY = y0, maxX = x0, maxY = y0;
    int16_t i = 0, j = 0;
    int vertexX[] = {x0, x1, x2};
    int vertexY[] = {y0, y1, y2};

    if (!isFilled)                                                              // 指定三角形不填充
    {
        // 调用画线函数，将三个点用直线连接
        OLED_DrawLine(x0, y0, x1, y1);
        OLED_DrawLine(x0, y0, x2, y2);
        OLED_DrawLine(x1, y1, x2, y2);
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
                if (pnpoly(3, vertexX, vertexY, i, j)) {OLED_DrawPoint(i, j);}
            }
        }
    }
}

/**
 * @brief OLED绘制圆
 * 
 * @param x 圆心所在的列，范围: 0 ~ 127
 * @param y 圆心所在的行，范围: 0 ~ 63
 * @param radius 半径
 * @param isFilled 是否要填充 1: 填充，0: 不填充
 */
void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t isFilled)
{
    // 使用Bresenham算法画圆，可以避免耗时的浮点运算，效率更高

    int16_t x_t = 0, y_t = radius;
    int16_t d = 1 - radius;
    int16_t j = 0;

    // 画每个八分之一圆弧的起始点
    OLED_DrawPoint(x + x_t, y + y_t);
    OLED_DrawPoint(x - x_t, y - y_t);
    OLED_DrawPoint(x + y_t, y + x_t);
    OLED_DrawPoint(x - y_t, y - x_t);

    if (isFilled)		                                                        // 指定圆填充
    {
        // 遍历起始点y坐标，在指定区域画点，填充部分圆
        for (j = -y_t; j < y_t; j++)
        {
            OLED_DrawPoint(x, y + j);
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
        OLED_DrawPoint(x + x_t, y + y_t);
        OLED_DrawPoint(x + y_t, y + x_t);
        OLED_DrawPoint(x - x_t, y - y_t);
        OLED_DrawPoint(x - y_t, y - x_t);
        OLED_DrawPoint(x + x_t, y - y_t);
        OLED_DrawPoint(x + y_t, y - x_t);
        OLED_DrawPoint(x - x_t, y + y_t);
        OLED_DrawPoint(x - y_t, y + x_t);
        
        if (isFilled)	                                                        // 指定圆填充
        {
            // 遍历中间部分，在指定区域画点，填充部分圆
            for (j = -y_t; j < y_t; j++)
            {
                OLED_DrawPoint(x + x_t, y + j);
                OLED_DrawPoint(x - x_t, y + j);
            }
            
            // 遍历两侧部分
            for (j = -x_t; j < x_t; j++)
            {
                // 在指定区域画点，填充部分圆
                OLED_DrawPoint(x - y_t, y + j);
                OLED_DrawPoint(x + y_t, y + j);
            }
        }
    }
}

/**
 * @brief OLED绘制椭圆函数
 * 
 * @param x 圆心所在的列，范围: 0 ~ 127
 * @param y 圆心所在的行，范围: 0 ~ 63
 * @param a 椭圆的横向半轴长度，范围: 0 ~ 127
 * @param b 椭圆的纵向半轴长度，范围: 0 ~ 63
 * @param isFilled 是否要填充 1: 填充，0: 不填充
 */
void OLED_DrawEllipse(uint8_t x, uint8_t y, uint8_t a, uint8_t b, uint8_t isFilled)
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
            OLED_DrawPoint(x, y + j);
            OLED_DrawPoint(x, y + j);
        }
    }

    // 画椭圆弧的起始点
    OLED_DrawPoint(x + x_t, y + y_t);
    OLED_DrawPoint(x - x_t, y - y_t);
    OLED_DrawPoint(x - x_t, y + y_t);
    OLED_DrawPoint(x + x_t, y - y_t);

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
                OLED_DrawPoint(x + x_t, y + j);
                OLED_DrawPoint(x - x_t, y + j);
            }
        }
        
        // 画椭圆中间部分圆弧
        OLED_DrawPoint(x + x_t, y + y_t);
        OLED_DrawPoint(x - x_t, y - y_t);
        OLED_DrawPoint(x - x_t, y + y_t);
        OLED_DrawPoint(x + x_t, y - y_t);
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
                OLED_DrawPoint(x + x_t, y + j);
                OLED_DrawPoint(x - x_t, y + j);
            }
        }
        
        /*画椭圆两侧部分圆弧*/
        OLED_DrawPoint(x + x_t, y + y_t);
        OLED_DrawPoint(x - x_t, y - y_t);
        OLED_DrawPoint(x - x_t, y + y_t);
        OLED_DrawPoint(x + x_t, y - y_t);
    }
}