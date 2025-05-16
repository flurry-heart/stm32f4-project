#ifndef __W5500_DEVICE_H__
#define __W5500_DEVICE_H__

#include "stm32f4xx_hal.h"

#include "wizchip_conf.h"
#include "socket.h"
#include "DHCP/dhcp.h"

#include "bsp_uart.h"
#include "bsp_systick.h"

#define W5500_CS_GPIO_PORT                      GPIOA
#define W5500_CS_GPIO_PIN                       GPIO_PIN_4
#define RCC_W5500_CS_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()

#define W5500_RESET_GPIO_PORT                   GPIOD
#define W5500_RESET_GPIO_PIN                    GPIO_PIN_8
#define RCC_W5500_RESET_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()

#define W5500_INTERRUPT_GPIO_PORT               GPIOD
#define W5500_INTERRUPT_GPIO_PIN                GPIO_PIN_9
#define RCC_W5500_INTERRUPT_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()

#define W5500_CS(x)                             do{ x ? \
                                                    HAL_GPIO_WritePin(W5500_CS_GPIO_PORT, W5500_CS_GPIO_PIN, GPIO_PIN_SET):\
                                                    HAL_GPIO_WritePin(W5500_CS_GPIO_PORT, W5500_CS_GPIO_PIN, GPIO_PIN_RESET);\
                                                }while(0)

#define W5500_RESET(x)                          do{ x ? \
                                                    HAL_GPIO_WritePin(W5500_RESET_GPIO_PORT, W5500_RESET_GPIO_PIN, GPIO_PIN_SET):\
                                                    HAL_GPIO_WritePin(W5500_RESET_GPIO_PORT, W5500_RESET_GPIO_PIN, GPIO_PIN_RESET);\
                                                }while(0)


#define DATA_BUFFER_SIZE        2048
#define DHCP_MAX_RETRY_COUNT    30

extern SPI_HandleTypeDef g_w5500_spi_handle;

extern struct wiz_NetInfo_t g_w5500_net_info;                                   // 网络信息
extern uint8_t g_w5500_data_buff[DATA_BUFFER_SIZE];                             // 数据缓冲区

void W5500_Init(SPI_HandleTypeDef *hspi);
void W5500_Reset(void);

void W5500_SetMac(void);
void W5500_SetIp(void);

void PrintInfo(void);

void DHCP_TryToGetIp(uint8_t socket);

#endif // !__DEVICE_H__