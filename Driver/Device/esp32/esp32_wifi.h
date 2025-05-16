#ifndef __ESP32_WIFI_H__
#define __ESP32_WIFI_H__

#include <stdio.h>
#include <string.h>
#include <regex.h>

#include "esp32.h"

#define WIFI_SSID               "HUAWEI-1AA2CE"                                 // WiFi名字
#define WIFI_PWD                "12345678"                                      // WiFi密码

// 阿里云的账号CLIENT_ID和PASSWODR密码过一段时间就自动变
#define CLIEND_ID               "h716BiondGQ.D001|securemode=2,signmethod=hmacsha256,timestamp=1742910101818|"
#define USERNAME                "D001&h716BiondGQ"
#define PASSWORD                "ac0e0c474ecc11a23846837b6f4f8e91cdc70a5899bad1b1323304df49777bc7"

#define MQTT_HOST_URL           "iot-06z00c5zp62mf64.mqtt.iothub.aliyuncs.com"
#define PORT                    "1883"

// 属性上报主题: /sys/${productKey}/${deviceName}/thing/event/property/post
#define PUSBLISH_TOPIC          "/sys/h716BiondGQ/D001/thing/event/property/post"

extern uint8_t mqtt_message[512];

void ESP32_WiFi_Init(void);

void ESP32_WiFi_EnterUnvarnished(void);

void ESP32_WiFi_SetMode(uint8_t mode);
void ESP32_WiFi_Connect(char *ssid, char *pwd);
void ESP32_WiFi_GetIp(char *ip);

void ESP32_WiFi_SetConnectionCount(uint8_t mode);
void ESP32_WiFi_StartTcpServer(uint16_t port);

void ESP32_WiFi_ConnectTcpServer(char *server_ip, char *server_port);
void ESP32_WiFi_ReadTcpData(uint16_t *id, char ip[], uint16_t *port, char *data, uint16_t *length);


bool ESP32_WiFi_MQTT_KeepAlive(void);

#endif // !__ESP32_WIFI_H__