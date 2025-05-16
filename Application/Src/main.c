#include "stm32f4xx_hal.h"

#include "bsp_clock.h"
#include "bsp_systick.h"

#include "bsp_spi.h"
#include "bsp_uart.h"
#include "bsp_timer.h"

#include "w5500/w5500_tcp.h"

int main(void)
{
    HAL_Init();
    System_Clock_Init(8, 336, 2, 7);
    Delay_Init();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  
    BSP_UART_Init(&g_usart1_handle, USART1, 115200);
    BSP_SPI_Init(&g_spi1_handle, SPI1, SPI_POLARITY_LOW, SPI_PHASE_1EDGE, SPI_BAUDRATEPRESCALER_4, SPI_FIRSTBIT_MSB);
    BSP_Timer_Base_Init(&g_timer6_handle, TIM6, 8399, 9999);

    W5500_Init(&g_spi1_handle);
    wizchip_init(NULL, NULL);                                                   // 缓冲区默认

    __HAL_TIM_CLEAR_IT(&g_timer6_handle, TIM_IT_UPDATE); 
    HAL_TIM_Base_Start_IT(&g_timer6_handle);

    DHCP_TryToGetIp(0);
  
    PrintInfo();                                                                // 打印网络信息
  
    while (1)
    {
        W5500_TCP_Server(0, 8080);
    }
  
    return 0;
}