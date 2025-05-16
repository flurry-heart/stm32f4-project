#ifndef __LCD_H__
#define __LCD_H__

#include "stm32f4xx_hal.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "screen/color.h"
#include "screen/font.h"
#include "screen/image.h"
#include "geometry/geometry.h"

/* 
 * 我们一般使用FSMC的块1（BANK1）来驱动SRAM, 块1地址范围总大小为256MB,均分成4块:
 * 存储块1（FSMC_NE1）地址范围: 0X6000 0000 ~ 0X63FF FFFF
 * 存储块2（FSMC_NE2）地址范围: 0X6400 0000 ~ 0X67FF FFFF
 * 存储块3（FSMC_NE3）地址范围: 0X6800 0000 ~ 0X6BFF FFFF
 * 存储块4（FSMC_NE4）地址范围: 0X6C00 0000 ~ 0X6FFF FFFF
 */
#define LCD_FSMC_NEX         4                                                  // 使用FSMC_NE4接LCD_CS
#define LCD_FSMC_AX          6                                                  // 使用FSMC_A6接LCD_DC

#define LCD_BASE_ADDRESS    (0x60000000 + (0x4000000 * (LCD_FSMC_NEX - 1)))
#define LCD_CMD             *(uint16_t *)LCD_BASE_ADDRESS
#define LCD_DATA            *(uint16_t *)(LCD_BASE_ADDRESS | (1 << (LCD_FSMC_AX + 1)))

#define LCD_CS_GPIO_PORT                GPIOC
#define LCD_CS_GPIO_PIN                 GPIO_PIN_8
#define RCC_LCD_CS_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()

#define LCD_RS_GPIO_PORT                GPIOC
#define LCD_RS_GPIO_PIN                 GPIO_PIN_9
#define RCC_LCD_RS_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()

#define LCD_WR_GPIO_PORT                GPIOC
#define LCD_WR_GPIO_PIN                 GPIO_PIN_10
#define RCC_LCD_WR_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()

#define LCD_RD_GPIO_PORT                GPIOC
#define LCD_RD_GPIO_PIN                 GPIO_PIN_11
#define RCC_LCD_RD_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()

#define LCD_CS(x)                       (LCD_CS_GPIO_PORT->BSRR = LCD_CS_GPIO_PIN << (16 * (!x)))
#define LCD_RS(x)                       (LCD_RS_GPIO_PORT->BSRR = LCD_RS_GPIO_PIN << (16 * (!x)))
#define LCD_WR(x)                       (LCD_WR_GPIO_PORT->BSRR = LCD_WR_GPIO_PIN << (16 * (!x)))
#define LCD_RD(x)                       (LCD_RD_GPIO_PORT->BSRR = LCD_RD_GPIO_PIN << (16 * (!x)))

#define LCD_WIDTH   320
#define LCD_HEIGHT  480

typedef enum LCD_DisplayMode
{
    LCD_DISPLAY_NORMAL = 0,
    LCD_DISPLAY_OVERLAPPING = 1
} LCD_DisplayMode;

typedef struct LCD_DataFormat
{
    uint8_t digit;                                                              // 位数
    uint8_t decimal;                                                            // 小数位数
    uint8_t symbol : 1;                                                         // 是否显示符号
    uint8_t align_right : 1;                                                    // 是否右对齐
    uint8_t display_mode : 1;                                                   // 是否叠加显示
} LCD_DataFormat;

void LCD_Init(void);
void LCD_SetCursor(uint16_t x, uint16_t y);
void LCD_SetCursorArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DisplayDirection(uint8_t mode);
void LCD_Clear(uint16_t color);
void LCD_ClearArea(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y);

void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DisplayMode mode);
void LCD_ShowString(uint16_t x, uint16_t y, char *str, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DisplayMode mode);
void LCD_ShowChinese(uint16_t x, uint16_t y, char *chinese, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DisplayMode mode);
void LCD_ShowNumber(uint16_t x, uint16_t y, int num, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DataFormat dataFormat);
void LCD_ShowDecimal(uint16_t x, uint16_t y, double decimal, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DataFormat dataFormat);
void LCD_Printf(uint16_t x, uint16_t y, uint16_t size, uint16_t fColor, uint16_t bColor, LCD_DisplayMode mode, char *format, ...);

void LCD_ShowPicture(uint8_t image[], uint16_t x, uint16_t y, uint16_t width, uint16_t height);


void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void LCD_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color, uint8_t isFilled);
void LCD_DrawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t isFilled);
void LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color, uint8_t isFilled);
void LCD_DrawEllipse(uint16_t x, uint16_t y, uint16_t a, uint16_t b, uint16_t color, uint8_t isFilled);

#endif // !__LCD_H__