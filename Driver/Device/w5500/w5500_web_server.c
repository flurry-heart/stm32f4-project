#include "w5500_web_server.h"

uint8_t g_w5500_web_server_tx_buff[2048];
uint8_t g_w5500_web_server_rx_buff[2048];

uint8_t *g_w5500_web_server_socket_list;
uint8_t g_w5500_web_server_socket_count;

char *g_web_server_content_name = "index.html";
char *g_web_server_content = 
    "<!DOCTYPE html> \
    <html> \
        <head> \
            <title>W5500 Web Server</title> \
            <meta charset=\"utf-8\"></meta> \
        </head> \
        <body> \
            <a href=\"/index.html?action=1\"><button>开灯</button></a> \
            <a href=\"/index.html?action=2\"><button>关灯</button></a> \
        </body> \
    </html>";

/**
 * @brief W5500 Web服务器初始化
 * 
 * @param socket_list socket 列表
 * @param socket_count socket 个数
 * @param content_name 响应的内容文件名
 * @param content 响应的内容
 */
void W5500_WebServer_Init(uint8_t *socket_list, uint8_t socket_count, char *content_name, char *content)
{
    g_w5500_web_server_socket_list = socket_list;
    g_w5500_web_server_socket_count = socket_count;

    // 初始化HTTP服务器
    httpServer_init(g_w5500_web_server_tx_buff, g_w5500_web_server_rx_buff, socket_count, socket_list);

    // 注册HTML页面，表示服务器要响应的内容（网页）
    reg_httpServer_webContent((uint8_t *)content_name, (uint8_t *)content);
}

/**
 * @brief 启动W5500 Web服务器
 * 
 */
void W5500_WebServer_Start(void)
{
    // 处理HTTP服务器
    for (uint8_t i = 0; i < g_w5500_web_server_socket_count; i++)
    {
        httpServer_run(g_w5500_web_server_socket_list[i]);
    }
}

/**
 * @brief W55000 Web服务器用户自定义函数
 * 
 * @param url 返回的URL
 */
void handler_user_funcion(char *url)
{
    // 从URL里提取出参数
    char *pAction = strstr(url, "action=");
    if (pAction != NULL)
    {
        // 获取具体的参数值
        int action = 0;
        if (sscanf(pAction, "action=%d", &action) == 1)
        {
            switch (action)
            {
            case 1:
                // 打开灯
                LED_SetStatus(GPIOF, GPIO_PIN_9, LED_ON);
                break;
            case 2:
                // 关闭灯
                LED_SetStatus(GPIOF, GPIO_PIN_9, LED_OFF);
                break;
            default:
                break;
            }
        }
    }
}