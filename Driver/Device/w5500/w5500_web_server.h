#ifndef __W5500_WEB_SERVER_H__
#define __W5500_WEB_SERVER_H__

#include <stdio.h>
#include <string.h>

#include "httpServer/httpServer.h"

#include "led/led.h"

extern char *g_web_server_content_name;
extern char *g_web_server_content;

void W5500_WebServer_Init(uint8_t *socket_list, uint8_t socket_count, char *content_name, char *content);
void W5500_WebServer_Start(void);

#endif // !__W5500_WEB_SERVER_H__