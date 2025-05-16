#ifndef __ESP32_BLE_H__
#define __ESP32_BLE_H__

#include "esp32.h"

typedef enum ESP32_BLE_Status_t 
{
    WIFI_CHANGE,                                                                // WIFI状态变化
    BLE_ERROR,                                                                  // 错误
    BLE_CONN,                                                                   // 蓝牙连接
    BLE_DISCONN,                                                                // 蓝牙断开
    BLE_UT,                                                                     // 蓝牙透传模式
    BLE_DATA,                                                                   // 蓝牙数据模式
    BLE_OTHER,                                                                  // 其它
} ESP32_BLE_Status_t; 

void ESP32_BLE_Init(void);

ESP32_BLE_Status_t ESP32_BLE_HandlerConnectChange(void);
void ESP32_BLE_ReadData(char *data, uint16_t *length);
void ESP32_BLE_SendData(char *data, uint16_t length);

#endif // !__ESP32_BLE_H__