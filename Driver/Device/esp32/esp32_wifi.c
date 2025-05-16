#include "esp32_wifi.h"

uint8_t mqtt_message[512];

/**
 * @brief ESP32 WIFI功能初始化函数
 * 
 * @param huart 串口句柄
 * @param frameData 串口接收数据帧
 */
void ESP32_WiFi_Init(void)
{
    ESP32_ExitUnvarnished();                                                    // 退出透传模式
    ESP32_SendAtCmd("AT+SAVETRANSLINK=0", "OK", 500);                           // 关闭WIFI上电进入透传模式
    
                     
    if (ESP32_SendAtCmd("AT+RST", "OK", 3000))                                  // 重启WIFI芯片
    {
        printf("重启WIFI芯片成功\r\n");
        HAL_Delay(3000);
    }

    if (ESP32_SendAtCmd("AT+CWINIT=0", "OK", 500))                              // 初始化WIFI驱动程序
    {
        printf("初始化WIFI驱动程序成功\r\n");
    }
                                          
    if (ESP32_SendAtCmd("AT+RESTORE", "ready", 3000))                           // 恢复出场设置
    {
        printf("恢复出场设置成功\r\n");
    }
                   
    if (ESP32_SendAtCmd("AT", "OK", 500))                                       // 发送AT指令，等待是否回复OK
    {
        printf("AT指令测试成功\r\n");
    }
}

/**
 * @brief ESP32进入透传函数
 * 
 */
void ESP32_WiFi_EnterUnvarnished(void)
{
    if (ESP32_SendAtCmd("AT+CIPMODE=1", "OK", 500))
    {
        printf("设置最大连接数成功");
    }

    if (ESP32_SendAtCmd("AT+CIPSEND", ">", 500))
    {
        printf("进入透传模式成功\r\n");
    }
}


/**
 * @brief 设置ESP32工作模式函数
 * 
 * @param mode 1: Station模式; 2: AP模式; 3: AP+Station模式
 */
void ESP32_WiFi_SetMode(uint8_t mode)
{
    char cmd[15] = {0};
    char *modeStr[] = {"Station模式", "AP模式", "AP+Station模式"};

    if (mode < 1 || mode > 3)
    {
        printf("参数错误，工作模式设置失败\r\n");
        return;
    }

    sprintf(cmd, "AT+CWMODE=%d", mode);
    if (ESP32_SendAtCmd(cmd, "OK", 500))
    {
        printf("%s设置成功\r\n", modeStr[mode - 1]);
    }
}

/**
 * @brief ESP32连接WIFI函数
 * 
 * @param ssid  WIFI名称
 * @param pwd WIFI密码
 */
void ESP32_WiFi_Connect(char *ssid, char *pwd)
{
    char cmd[64] = {0};

    ESP32_WiFi_SetMode(1);                                                      // 设置工作模式为Station模式
    
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
    if (ESP32_SendAtCmd(cmd, "WIFI GOT IP", 10000))
    {
        printf("WIFI连接成功\r\n");
    }
}

/**
 * @brief ESP32获取IP地址函数
 * 
 * @param ip IP地址，需要16字节内存空间
 * @return uint8_t 0: 获取IP地址成功; 1: 获取IP地址失败;
 */
void ESP32_WiFi_GetIp(char *ip)
{
    char *p_start;
    char *p_end;
    
    if (ESP32_SendAtCmd("AT+CIFSR", "OK", 500))
    {
        printf("获取IP地址成功\r\n");
    }
    
    p_start = strstr((const char *)pg_uart_esp32_frameData->data, "\"");
    p_end = strstr(p_start + 1, "\"");
    *p_end = '\0';
    sprintf(ip, "%s", p_start + 1);
}

/**
 * @brief ESP32设置是否多连接函数
 * 
 * @param mode 0: 单连接模式; 1: 多连接模式;
 */
void ESP32_WiFi_SetConnectionCount(uint8_t mode)
{
    char cmd[15] = {0};
    char *modeStr[] = {"单连接模式", "多连接模式",};

    if (mode != 0 && mode != 1)
    {
        printf("参数错误，连接模式设置失败\r\n");
        return;
    }

    sprintf(cmd, "AT+CIPMUX=%d", mode);
    if (ESP32_SendAtCmd(cmd, "OK", 500))                                        // 设置是否多连接
    {
        printf("%s设置成功\r\n", modeStr[mode]);
    }
}

/**
 * @brief 开启TCP服务器
 * 
 * @param port 端口号
 */
void ESP32_WiFi_StartTcpServer(uint16_t port)
{
    char cmd[30] = {0};
    ESP32_WiFi_SetConnectionCount(1);                                           // 开启多连接

    if (ESP32_SendAtCmd("AT+CIPDINFO=1", "OK", 500))                            // 设置IPD消息格式
    {
        printf("设置IPD消息格式成功\r\n");
    }

    sprintf(cmd, "AT+CIPSERVER=1,%d,\"TCP\"", port);
    if (ESP32_SendAtCmd(cmd, "OK", 3000))                                       // 开启TCP服务器
    {
        printf("开启TCP服务器成功\r\n");
    }
}

/**
 * @brief ESP32读取TCP通信发送过来的数据
 * 
 * @param id 对方的id
 * @param ip 对方的ip地址
 * @param port 对方的端口号
 * @param data 对方要发送的数据
 * @param length 对方发送的数据长度
 */
void ESP32_WiFi_ReadTcpData(uint16_t *id, char ip[], uint16_t *port, char *data, uint16_t *length)
{
    if (pg_uart_esp32_frameData->finsh)
    {
        if (strstr((const char *)pg_uart_esp32_frameData->data, "+IPD"))        // 收到TCP传输的数据
        {
            // 格式类型: \r\n+IPD,第几个连接,数据长度,"IP地址",端口号:数据\r\n,
            memset(data, 0, strlen(data));
            sscanf((const char *)pg_uart_esp32_frameData->data, "%*[\r\n]+IPD,%hd,%hd,\"%[^\"]\",%hd", id, length, ip, port);
            strtok((char *)pg_uart_esp32_frameData->data, ":");
            memcpy(data, strtok(NULL, ":"), *length);
        }
        
        BSP_UART_ClearFrameData(pg_uart_esp32_frameData);
    }
}

/**
 * @brief ESP32通过TCP通信发送数据
 * 
 * @param id 对方的id
 * @param data 对方要发送的数据
 * @param length 对方发送的数据长度
 */
void ESP32_WiFi_SendTcpData(uint16_t id, char *data, uint16_t length)
{
    char cmd[30] = {0};

    if (length <= 0 || strcmp(data, "") == 0)
    {
        return;
    }

    sprintf(cmd, "AT+CIPSEND=%d,%d", id, length);
    if (ESP32_SendAtCmd(cmd, "OK", 500))
    {
        if (ESP32_SendAtCmd(data, "OK", 3000))
        {
            printf("发送TCP数据成功\r\n");
        } 
    }
}

/**
 * @brief ESP32连接TCP服务器
 * 
 * @param server_ip TCP服务器IP地址
 * @param server_port TCP服务器端口号
 */
void ESP32_WiFi_ConnectTcpServer(char *server_ip, char *server_port)
{
    char cmd[128] = {0};
    
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", server_ip, server_port);
    if (ESP32_SendAtCmd(cmd, "CONNECT", 5000))
    {
        printf("连接TCP服务器成功\r\n");
    }
}

/**
 * @brief ESP32发送MQTT心跳包
 * 
 * @return true 心跳包发送成功，不需要断网重连
 * @return false 心跳包发送失败，需要断网重连
 */
bool ESP32_WiFi_MQTT_KeepAlive(void)
{
    uint8_t i = 0;
    uint8_t message[2] = {0xC0, 0x00};

    for (i = 0; i < 5; i++)
    {
        HAL_UART_Transmit(pg_uart_esp32_handler, message, 2, 0xFFFF);
        if (pg_uart_esp32_frameData->finsh == 1)
        {
            if (pg_uart_esp32_frameData->data[0] == 0xD0 && pg_uart_esp32_frameData->data[1] == 0x00)
            {
                break;
            }
        }
        BSP_UART_ClearFrameData(pg_uart_esp32_frameData);
        HAL_Delay(1000);
    }

    BSP_UART_ClearFrameData(pg_uart_esp32_frameData);

    return (i == 5) ? false : true;
}