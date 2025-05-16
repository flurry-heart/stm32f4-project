#ifndef __OLED_H__
#define __OLED_H__

#include "stm32f4xx_hal.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "bsp_spi.h"
#include "screen/font.h"
#include "geometry/geometry.h"

typedef enum OLED_WriteMode
{
    OLED_MODE_CMD = 0,
    OLED_MODE_DATA = 1
} OLED_WriteMode;

typedef struct DataFormat
{
    uint8_t digit;                                                              // 位数
    uint8_t decimal;                                                            // 小数位数
    uint8_t symbol : 1;                                                         // 是否显示符号
    uint8_t align_right : 1;                                                    // 是否右对齐
} DataFormat;

#define OLED_CS_GPIO_PORT                   GPIOD
#define OLED_CS_GPIO_PIN                    GPIO_PIN_8
#define RCC_OLED_CS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()

#define OLED_CS(x)                          do{ x ? \
                                                HAL_GPIO_WritePin(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PIN, GPIO_PIN_SET):\
                                                HAL_GPIO_WritePin(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PIN, GPIO_PIN_RESET);\
                                            }while(0)

#define OLED_DC_GPIO_PORT                   GPIOD
#define OLED_DC_GPIO_PIN                    GPIO_PIN_9
#define RCC_OLED_DC_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()

#define OLED_DC(x)                          do{ x ? \
                                                HAL_GPIO_WritePin(OLED_DC_GPIO_PORT, OLED_DC_GPIO_PIN, GPIO_PIN_SET):\
                                                HAL_GPIO_WritePin(OLED_DC_GPIO_PORT, OLED_DC_GPIO_PIN, GPIO_PIN_RESET);\
                                            }while(0)

#define OLED_RESET_GPIO_PORT                GPIOD
#define OLED_RESET_GPIO_PIN                 GPIO_PIN_10
#define RCC_OLED_RESET_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()

#define OLED_RESET(x)                       do{ x ? \
                                                HAL_GPIO_WritePin(OLED_RESET_GPIO_PORT, OLED_RESET_GPIO_PIN, GPIO_PIN_SET):\
                                                HAL_GPIO_WritePin(OLED_RESET_GPIO_PORT, OLED_RESET_GPIO_PIN, GPIO_PIN_RESET);\
                                            }while(0)

extern SPI_HandleTypeDef *g_oled_spi_handle_ptr;

void OLED_Init(SPI_HandleTypeDef *hspi);
void OLED_RefreshGRAM(void);

void OLED_DisplayOn(void);
void OLED_DisplayOff(void);
void OLED_SetCursor(uint8_t x, uint8_t y);
void OLED_Clear(void);
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void OLED_DrawPoint(uint8_t x, uint8_t y);
void OLED_ShowImage(uint8_t x, uint8_t y, uint8_t width, uint8_t hight, const uint8_t * image);

void OLED_ShowChar(uint8_t x, uint8_t y, char chr, uint8_t size);
void OLED_ShowString(uint8_t x, uint8_t y, char * str, uint8_t size);
void OLED_ShowChinese(uint8_t x, uint8_t y, char * chinese, uint8_t size);
void OLED_ShowNumber(uint8_t x, uint8_t y, int num, uint8_t size, DataFormat dataFormat);
void OLED_ShowDecimal(uint8_t x, uint8_t y, double decimal, uint8_t size, DataFormat dataFormat);
void OLED_Printf(uint8_t x, uint8_t y, uint8_t size, char *format, ...);

void OLED_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t isFilled);
void OLED_DrawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t isFilled);
void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t isFilled);
void OLED_DrawEllipse(uint8_t x, uint8_t y, uint8_t a, uint8_t b, uint8_t isFilled);

#endif // !__OLED_H__