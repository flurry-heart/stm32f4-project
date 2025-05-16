#ifndef __W5500_UDP_H__
#define __W5500_UDP_H__

#include "socket.h"

#include "bsp_uart.h"

#include "w5500/w5500_device.h"

void W5500_UDP(uint8_t socket_index, uint16_t port);

void UDP_SendData(uint8_t socket_index, uint8_t *data, uint16_t length, uint8_t *targetIP, uint16_t targetPort);
void UDP_ReceiveData(uint8_t socket_index, uint8_t *data, uint16_t *length, uint8_t *targetIP, uint16_t *targetPort);

#endif // !__UDP_H__