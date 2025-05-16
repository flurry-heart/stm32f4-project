#ifndef __W5500_TCP_H__
#define __W5500_TCP_H__

#include "socket.h"

#include "bsp_uart.h"

#include "w5500/w5500_device.h"

#include "mqtt/mqtt.h"

void W5500_TCP_Server(uint8_t socket_index, uint16_t monitor_port);
void W5500_TCP_Client(uint8_t socket_index, uint16_t port, uint8_t *server_ip, uint16_t server_port);
void W5500_ConnectCloudServer(uint8_t socket_index, uint16_t port, uint8_t *server_ip, uint16_t server_port, char *client_id, char *username, char *password);
uint8_t W5500_MQTT_KeepAlive(uint8_t socket_index);

void TCP_SendData(uint8_t socket_index, uint8_t *data, uint16_t length);
void TCP_ReceiveData(uint8_t socket_index, uint8_t *data, uint16_t *length);

#endif // !__TCP