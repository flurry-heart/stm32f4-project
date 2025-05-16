#include "w5500_tcp.h"

uint8_t g_w5500_connect_cloud_status = 0;

/**
 * @brief TCP服务器
 * 
 * @param socket_index socket索引
 * @param monitor_port 监听的端口
 */
void W5500_TCP_Server(uint8_t socket_index, uint16_t monitor_port)
{
    // 获取Socket状态，参数：要获取的socket索引，0 ~ 7
    switch (getSn_SR(socket_index))
    {
    case SOCK_CLOSED:                                                           // 表示Socket已经关闭了
        // 打开socket，
        // 第一个参数是要创建的socket索引，第二个参数是使用的协议，第三个参数是端口，第四个参数是使用SF_TCP_NODELAY表示无延迟响应
        // 如果成功，返回socket索引
        if (socket(socket_index, Sn_MR_TCP, monitor_port, SF_TCP_NODELAY) == socket_index)
        {
            printf("socket %d 打开成功\r\n", socket_index);
        }
        else
        {
            printf("socket %d 打开失败\r\n", socket_index);
        }
        break;
    
    case SOCK_INIT:                                                             // 表示Socket已经打开了
        // 监听，参数是socket索引
        switch (listen(socket_index))
        {
        case SOCK_OK:
            printf("socket %d 监听成功\r\n", socket_index);
            break;
        
        case SOCKERR_SOCKINIT:
            printf("还未初始化 socket %d\r\n", socket_index);
            break;
        
        case SOCKERR_SOCKCLOSED:
            printf("socket %d 意外关闭\r\n", socket_index);
            break;

        default:
            printf("socket %d 监听失败\r\n", socket_index);
            break;
        }
        break;

    case SOCK_ESTABLISHED:                                                      // 表示Socket连接建立成功
        uint8_t cliendIP[4] = {0};
        uint16_t cliendPort = 0;
        uint16_t length = 0;

        // 获取目标的IP地址和端口号
        getSn_DIPR(socket_index, cliendIP);
        cliendPort = getSn_DPORT(socket_index);
        printf("客户端（%d.%d.%d.%d: %d）建立连接\r\n", cliendIP[0], cliendIP[1], cliendIP[2], cliendIP[3], cliendPort);

        // 在这里实现正常通信
        while (1)
        {
            // 等待客户端发送信息
            while ((getSn_IR(socket_index) & Sn_IR_RECV) == 0)                  // 通过中断寄存器的REVE位来判断
            {
                // 在客户端发送信息时，客户端有可能断开，连接状态发生变化
                if (getSn_SR(socket_index) != SOCK_ESTABLISHED)
                {
                    printf("socket %d 连接发生变化\r\n", socket_index);
                    close(socket_index);                                        // 关闭Socket
                    return;
                }
            }
            setSn_IR(socket_index, Sn_IR_RECV);                                 // 接收到数据了，清除中断标志位，写1清除，写0无效
            length = getSn_RX_RSR(socket_index);                                // 获取接收到数据长度
            if (length > 0)
            {
                memset(g_w5500_data_buff, 0, length + 1);
                recv(socket_index, g_w5500_data_buff, length);                  // 接收数据
                printf("客户端（%d.%d.%d.%d: %d）发来数据：%s\r\n", cliendIP[0], cliendIP[1], cliendIP[2], cliendIP[3], cliendPort, g_w5500_data_buff);
                send(socket_index, g_w5500_data_buff, length);                  // 发送回客户端数据
            }
        }
        break;

    case SOCK_CLOSE_WAIT:
        // 如果处于半不关闭状态，直接关闭socket
        printf("服务端（%d.%d.%d.%d）异常关闭\r\n", g_w5500_net_info.ip[0], g_w5500_net_info.ip[1], g_w5500_net_info.ip[2], g_w5500_net_info.ip[3]);
        close(socket_index);

    default:
        break;
    }
}

/**
 * @brief TCP客户端
 * 
 * @param socket_index socket索引 
 * @param port 自身的端口
 * @param server_ip 服务器ip地址
 * @param server_port 服务器的端口
 */
void W5500_TCP_Client(uint8_t socket_index, uint16_t port, uint8_t *server_ip, uint16_t server_port)
{
    // 获取Socket状态，参数：要获取的socket索引，0 ~ 7
    switch (getSn_SR(socket_index))
    {
    case SOCK_CLOSED:                                                           // 表示Socket已经关闭了
        // 打开socket，
        // 第一个参数是要创建的socket索引，第二个参数是使用的协议，第三个参数是端口，第四个参数是使用SF_TCP_NODELAY表示无延迟响应
        // 如果成功，返回socket索引
        if (socket(socket_index, Sn_MR_TCP, port, SF_TCP_NODELAY) == socket_index)
        {
            printf("socket %d 打开成功\r\n", socket_index);
        }
        else
        {
            printf("socket %d 打开失败\r\n", socket_index);
        }
        break;
  
    case SOCK_INIT:                                                             // 表示Socket已经打开了
        if (connect(socket_index, server_ip, server_port) == SOCK_OK)            // 作为客户端主动连接服务器
        {
            printf("socket %d 连接服务器成功\r\n", socket_index);
        }
        else
        {
            close(socket_index);                                                // 关闭socket
            printf("socket %d 连接服务器失败\r\n", socket_index);
        }
        break;

    case SOCK_ESTABLISHED:                                                      // 表示Socket连接建立成功
        uint16_t length = 0;

        printf("连接服务端（%d.%d.%d.%d: %d）成功\r\n", server_ip[0], server_ip[1], server_ip[2], server_ip[3], server_port);

        // 客户端往服务端发送数据
        send(socket_index, (uint8_t *)"Hello World!", 12);

        // 在这里实现正常通信
        while (1)
        {
            // 等待服务端返回信息
            while ((getSn_IR(socket_index) & Sn_IR_RECV) == 0)                  // 通过中断寄存器的REVE位来判断
            {
                // 在服务端返回信息时，服务端有可能断开，连接状态发生变化
                if (getSn_SR(socket_index) != SOCK_ESTABLISHED)
                {
                    printf("socket %d 连接发生变化\r\n", socket_index);
                    close(socket_index);                                        // 关闭Socket
                    return;
                }
            }
            setSn_IR(socket_index, Sn_IR_RECV);                                 // 接收到数据了，清除中断标志位，写1清除，写0无效
            length = getSn_RX_RSR(socket_index);                                // 获取接收到数据长度
            if (length > 0)
            {
                memset(g_w5500_data_buff, 0, length + 1);
                recv(socket_index, g_w5500_data_buff, length);                  // 接收数据
                printf("服务端（%d.%d.%d.%d: %d）返回数据：%s\r\n", server_ip[0], server_ip[1], server_ip[2], server_ip[3], server_port, g_w5500_data_buff);
                send(socket_index, g_w5500_data_buff, length);                  // 发送回服务端数据
            }
        }
        break;

    case SOCK_CLOSE_WAIT:
        // 如果处于半不关闭状态，直接关闭socket
        printf("客户端（%d.%d.%d.%d: %d）异常关闭\r\n", g_w5500_net_info.ip[0], g_w5500_net_info.ip[1], g_w5500_net_info.ip[2], g_w5500_net_info.ip[3], port);
        close(socket_index);

    default:
        break;
    }
}

/**
 * @brief W5500连接云服务器
 * 
 * @param socket_index socket索引 
 * @param port 自身的端口
 * @param server_ip 服务器ip地址
 * @param server_port 服务器的端口
 * @param client_id 客户端ID
 * @param username 用户名
 * @param password 密码
 */
void W5500_ConnectCloudServer(uint8_t socket_index, uint16_t port, uint8_t *server_ip, uint16_t server_port, char *client_id, char *username, char *password)
{
    // 获取Socket状态，参数：要获取的socket索引，0 ~ 7
    switch (getSn_SR(socket_index))
    {
    case SOCK_CLOSED:                                                           // 表示Socket已经关闭了
        // 打开socket，
        // 第一个参数是要创建的socket索引，第二个参数是使用的协议，第三个参数是端口，第四个参数是使用SF_TCP_NODELAY表示无延迟响应
        // 如果成功，返回socket索引
        if (socket(socket_index, Sn_MR_TCP, port, SF_TCP_NODELAY) == socket_index)
        {
            printf("socket %d 打开成功\r\n", socket_index);
        }
        else
        {
            printf("socket %d 打开失败\r\n", socket_index);
        }
        g_w5500_connect_cloud_status = 0;
        break;
  
    case SOCK_INIT:                                                             // 表示Socket已经打开了
        if (connect(socket_index, server_ip, server_port) == SOCK_OK)           // 作为客户端主动连接服务器
        {
            printf("socket %d 连接服务器成功\r\n", socket_index);
        }
        else
        {
            close(socket_index);                                                // 关闭socket
            printf("socket %d 连接服务器失败\r\n", socket_index);
        }
        g_w5500_connect_cloud_status = 0;
        break;

    case SOCK_ESTABLISHED:                                                      // 表示Socket连接建立成功
        uint16_t length = 0;

        if (g_w5500_connect_cloud_status == 0)
        {
            memset(g_w5500_data_buff, 0, DATA_BUFFER_SIZE);
            length = MQTT_ConnectMessage(g_w5500_data_buff, client_id, username, password);
            // 客户端往服务端发送数据，返回成功发送的数据大小
            if (send(socket_index, g_w5500_data_buff, length) == length)
            {
                // 等待服务端发送信息
                while ((getSn_IR(socket_index) & Sn_IR_RECV) == 0)              // 通过中断寄存器的REVE位来判断
                {
                    // 在服务端发送信息时，服务端有可能断开，连接状态发生变化
                    uint8_t status = getSn_SR(socket_index);
                    if (status != SOCK_ESTABLISHED)
                    {
                        printf("socket %d 连接发生变化，状态值为：%#x\r\n", socket_index, status);
                        close(socket_index);                                    // 关闭Socket
                        g_w5500_connect_cloud_status = 0;
                        return;
                    }
                }
                setSn_IR(socket_index, Sn_IR_RECV);                             // 接收到数据了，清除中断标志位，写1清除，写0无效
                
                length = getSn_RX_RSR(socket_index);                            // 获取接收到数据长度
                
                if (length == 4)
                {
                    memset(g_w5500_data_buff, 0, length + 1);
                    recv(socket_index, g_w5500_data_buff, length);              // 接收数据
                    
                    if (g_w5500_data_buff[0] == 0x20 && g_w5500_data_buff[1] == 0x02)
                    {
                        printf("阿里云连接成功\r\n");
                        g_w5500_connect_cloud_status = 1;
                    }
                    else
                    {
                        printf("阿里云连接失败\r\n");
                        close(socket_index);                                    // 关闭socket
                        g_w5500_connect_cloud_status = 0;
                    }
                } 
            }
        }
        
        break;

    default:
        break;
    }
}

/**
 * @brief W5500 MQTT保活函数
 * 
 * @param socket_index socket索引
 * @return uint8_t  0: 心跳包发送失败，需要断网重连; 1: 心跳包发送成功，不需要断网重连
 */
uint8_t W5500_MQTT_KeepAlive(uint8_t socket_index)
{
    uint8_t i = 0;
    uint8_t message[2] = {0xC0, 0x00};
    uint8_t response[10] = {0};
    uint8_t response_length = 0;
            
    if (g_w5500_connect_cloud_status == 1)
    {
        for (i = 0; i < 5; i++)
        {
            // 客户端往服务端发送数据，返回成功发送的数据大小
            if (send(socket_index, message, 2) == 2)
            {
                // 等待服务端发送信息
                while ((getSn_IR(socket_index) & Sn_IR_RECV) == 0)              // 通过中断寄存器的REVE位来判断
                {
                    // 在服务端发送信息时，服务端有可能断开，连接状态发生变化
                    uint8_t status = getSn_SR(socket_index);
                    if (status != SOCK_ESTABLISHED)
                    {
                        printf("socket %d 连接发生变化，状态值为：%#x\r\n", socket_index, status);
                        close(socket_index);                                    // 关闭Socket
                        g_w5500_connect_cloud_status = 0;
                        return 0;
                    }
                }

                setSn_IR(socket_index, Sn_IR_RECV);                             // 接收到数据了，清除中断标志位，写1清除，写0无效
                
                response_length = getSn_RX_RSR(socket_index);                   // 获取接收到数据长度
                
                if (response_length == 2)
                {
                    recv(socket_index, response, response_length);              // 接收数据
                    
                    if (response[0] == 0xD0 && response[1] == 0x00)
                    {
                        printf("心跳响应成功\r\n");
                        break;
                    }
                } 
            }

            Delay_ms(1000);
        }
    }

    return (i == 5) ? 0 : 1;
}

/**
 * @brief TCP发送数据
 * 
 * @param socket_index socket索引
 * @param data 发送数据缓冲区
 * @param length 发送数据长度
 */
void TCP_SendData(uint8_t socket_index, uint8_t *data, uint16_t length)
{                                 
    if (getSn_SR(socket_index) == SOCK_ESTABLISHED)                             // 获取Socket状态
    {
        send(socket_index, data, length);                                       // 发送数据
    }
}

/**
 * @brief TCP接收数据
 * 
 * @param socket_index socket索引
 * @param data 保存接收数据的缓冲区
 * @param length 接收数据长度
 */
void TCP_ReceiveData(uint8_t socket_index, uint8_t *data, uint16_t *length)
{                           
    if (getSn_SR(socket_index) == SOCK_ESTABLISHED)                             // 获取Socket状态
    {
        if ((getSn_IR(socket_index) & Sn_IR_RECV))                              // 根据事件标志判断是否接收到数据
        {
            setSn_IR(socket_index, Sn_IR_RECV);                                 // 清除中断标志位，写1清除，写0无效
            *length = getSn_RX_RSR(socket_index);                               // 获取接收到数据长度
            if (*length > 0)
            {
                recv(socket_index, data, *length);                              // 接收数据
            }
        }
    }
}