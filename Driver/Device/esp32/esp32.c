#include "esp32.h"

UART_HandleTypeDef *pg_uart_esp32_handler;
UART_FrameData_t *pg_uart_esp32_frameData;

/**
 * @brief ESP32初始化函数
 * 
 * @param huart 串口句柄
 * @param frameData 串口接收数据帧
 */
void ESP32_Init(UART_HandleTypeDef *huart, UART_FrameData_t *frameData)
{
    pg_uart_esp32_handler = huart;
    pg_uart_esp32_frameData = frameData;
}

/**
 * @brief ESP32发送AT指令函数
 * 
 * @param cmd 待发送的AT指令
 * @param ack 期待的应答结果
 * @param timeOut 等待超时时间
 * @return true 应答成功
 * @return false 应答失败
 */
bool ESP32_SendAtCmd(char *cmd, char *ack, uint32_t timeOut)
{
    BSP_UART_ClearFrameData(pg_uart_esp32_frameData);                           // 清除串口的帧数据
    printf("cmd:%s\r\n", cmd);                                                  // 打印AT指令
    BSP_UART_Printf(pg_uart_esp32_handler, "%s\r\n", cmd);                      // 发送AT指令
    
    if ((ack == NULL) || (timeOut == 0))
    {
        return 0;
    }
    else
    {
        while (timeOut > 0)
        {
            if (pg_uart_esp32_frameData->finsh)                                 // 判断是否接收完成
            {
                if (strstr((char *)pg_uart_esp32_frameData->data, ack) != NULL) // 获取数据帧中是否包含期待的应答结果
                {
                    return true;
                }
                else
                {
                    BSP_UART_ClearFrameData(pg_uart_esp32_frameData);           // 如果没有，则清空数据帧，等待下次接收
                }
            }
            timeOut--;
            HAL_Delay(1);
        }
        return false;
    }
}

/**
 * @brief ESP32设置回显模式函数
 * 
 * @param echo 0: 关闭回显; 1: 打开回显;
 * @return uint8_t 0: 设置回显模式成功; 1: 设置回显模式失败; 3: 参数错误，回显模式设置失败;
 */
void ESP32_EchoConfig(uint8_t echo)
{
    char cmd[15] = {0};
    char *echoStr[] = {"关闭回显成功", "打开回显成功",};

    if (echo != 0 && echo != 1)
    {
        printf("参数错误，回显模式设置失败\r\n");
        return;
    }

    sprintf(cmd, "ATE%d", echo);
    if (ESP32_SendAtCmd(cmd, "OK", 500))                                        // 设置回显模式
    {
        printf("%s\r\n", echoStr[echo]);
    }
}

/**
 * @brief ESP32退出透传函数
 * 
 */
void ESP32_ExitUnvarnished(void)
{
    HAL_Delay(20);
    BSP_UART_Printf(pg_uart_esp32_handler, "+++");
    HAL_Delay(20);

    printf("退出透传模式成功\r\n");
}