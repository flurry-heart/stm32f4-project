#ifndef __MQTT_H__
#define __MQTT_H__

#include <string.h>
#include <stdint.h>

uint16_t MQTT_ConnectMessage(uint8_t*mqtt_message,char *client_id,char *username,char *password);
uint16_t MQTT_PublishMessage(uint8_t * mqtt_message, char * topic, char * message, uint8_t udp, uint8_t QoS, uint8_t retain);

#endif // !__MQTT_H__