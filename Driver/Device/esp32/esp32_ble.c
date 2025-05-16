#include "esp32_ble.h"

/**
 * @brief ESP32的蓝牙功能初始化函数
 * 
 */
void ESP32_BLE_Init(void)
{    
    // 初始化蓝牙角色 1: client; 2: server
    if (ESP32_SendAtCmd("AT+BLEINIT=2", "OK", 3000))                            // 低功耗蓝牙初始化
    {
        printf("低功耗蓝牙初始化成功\r\n");
    }

    if (ESP32_SendAtCmd("AT+BLEGATTSSRVCRE", "OK", 3000))                       // 创建GSTT服务
    {
        printf("创建GSTT服务成功\r\n");
    }

    if (ESP32_SendAtCmd("AT+BLEGATTSSRVSTART","OK", 500))                       // 开启GSTT服务
    {
        printf("开启GSTT服务成功\r\n");
    }

    // AT+BLEADVPARAM=<最小广播间隔>,<最大广播间隔>,<adv_type>,<Bluetooth LE 地址类型>,<广播信道>[,<广播过滤器规则>][,<对方 Bluetooth LE 地址类型>,<对方 Bluetooth LE 地址>]
    if (ESP32_SendAtCmd("AT+BLEADVPARAM=50,50,0,0,7,0", "OK", 500))             // 设置蓝牙广播参数
    {
        printf("设置蓝牙广播参数成功\r\n");
    }

    // AT+BLEADVDATAEX=<设备名称>,<uuid>,<制造商数据>,<是包含TX功率>
    if (ESP32_SendAtCmd("AT+BLEADVDATAEX=\"ESP32\",\"A111\",\"1122334455\",1", "OK", 500))  // 自动设置广播参数
    {
        printf("自动设置广播参数成功\r\n");
    }
    
    if (ESP32_SendAtCmd("AT+BLEADVSTART", "OK", 500))                           // 开启蓝牙广播
    {
        printf("开启蓝牙广播成功\r\n");
    }

    if (ESP32_SendAtCmd("AT+SYSMSG=4", "OK", 500))                              // 设置系统提示信息
    {
        printf("设置系统提示信息成功\r\n");
    }
    
    // 开启透传要在连接之后
    if (ESP32_SendAtCmd("AT+BLESPPCFG=1,1,7,1,5", "OK", 300))                   // 设置蓝牙透传参数
    {
        printf("设置蓝牙透传参数成功\r\n");
    }
}

/**
 * @brief ESP32蓝牙连接状态改变的处理函数
 * 
 * @return ESP32_BLE_Status ESP32连接状态枚举值
 */
ESP32_BLE_Status_t ESP32_BLE_HandlerConnectChange(void)
{
    // 如果还没有接收完，直接结束方法
    if (!pg_uart_esp32_frameData->finsh)
    {
        return BLE_ERROR;
    }

    if (strstr((const char *)pg_uart_esp32_frameData->data, "WIFI"))
    {
        printf("WIFI连接状态改变\r\n");
        BSP_UART_ClearFrameData(pg_uart_esp32_frameData);
        return WIFI_CHANGE;
    }
    else if (strstr((const char *)pg_uart_esp32_frameData->data, "+BLECONN:"))
    {
        printf("蓝牙连接成功，准备开启透传模式\r\n");
        if (ESP32_SendAtCmd("AT+BLESPP", "OK", 500))                           
        {
            printf("开启透传模式成功\r\n");
        }
        BSP_UART_ClearFrameData(pg_uart_esp32_frameData);
        return BLE_UT;
    }
    else if (strstr((const char *)pg_uart_esp32_frameData->data, "+BLEDISCONN"))
    {
        printf("蓝牙已断开连接，准备退出透传模式\r\n");
        BSP_UART_ClearFrameData(pg_uart_esp32_frameData);

        ESP32_ExitUnvarnished();                                                // 退出透传模式

        printf("重新开始广播\r\n");
        if (ESP32_SendAtCmd("AT+BLEADVSTART", "OK", 500))                       // 开启蓝牙广播
        {
            printf("重新开启蓝牙广播成功\r\n");
        }
        BSP_UART_ClearFrameData(pg_uart_esp32_frameData);
        return BLE_DISCONN;
    }
    else if (strstr((const char *)pg_uart_esp32_frameData->data, "+BLECONNPARAM"))
    {
        BSP_UART_ClearFrameData(pg_uart_esp32_frameData);
        return BLE_OTHER;
    }
    
    return BLE_DATA;
}

/**
 * @brief ESP32读取蓝牙接收的数据
 * 
 * @param data 用于保存接收到的数据
 * @param length 记录接收数据的长度
 */
void ESP32_BLE_ReadData(char *data, uint16_t *length)
{
    memset(data, 0, strlen(data));
    *length = 0;

    // 如果还没有接收完，直接结束方法
    if (!pg_uart_esp32_frameData->finsh)
    {
        return;
    }

    if (ESP32_BLE_HandlerConnectChange() == BLE_DATA)
    {
        strcpy(data, (const char *)pg_uart_esp32_frameData->data);
        *length = pg_uart_esp32_frameData->length;
    }
    
    BSP_UART_ClearFrameData(pg_uart_esp32_frameData);
}

/**
 * @brief ESP32通过蓝牙发送的数据
 * 
 * @param data 指向要发送数据的指针
 * @param length 要发送数据的长度
 */
void ESP32_BLE_SendData(char *data, uint16_t length)
{
    if (length == 0)
    {
        return;
    }
    
    HAL_UART_Transmit(pg_uart_esp32_handler, (uint8_t *)data, length, 100);
}