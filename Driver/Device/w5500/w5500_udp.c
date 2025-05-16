#include "w5500_udp.h"

/**
 * @brief UDP通信
 * 
 * @param socket_index socket的索引
 * @param port 端口号
 */
void W5500_UDP(uint8_t socket_index, uint16_t port)
{
    // 获取Socket状态，参数：要获取的socket索引，0 ~ 7
    switch (getSn_SR(socket_index))
    {
    case SOCK_CLOSED:                                                           // 表示Socket已经关闭了
        // 打开socket，
        // 第一个参数是要创建的socket索引，第二个参数是使用的协议，第三个参数是端口，第四个参数是使用SF_TCP_NODELAY表示无延迟响应
        // 如果成功，返回socket索引
        if (socket(socket_index, Sn_MR_UDP, port, 0) == socket_index)
        {
            printf("socket %d 打开成功\r\n", socket_index);
        }
        else
        {
            printf("socket %d 打开失败\r\n", socket_index);
        }
        break;
    
    case SOCK_UDP:                                                              // 表示Socket已经打开了
        uint8_t length = 0;
        uint8_t targetIP[4];
        uint16_t targetPort = 0;

        while (1)
        {
            while ((getSn_IR(socket_index) & Sn_IR_RECV) == 0)                  // 等待接收数据
            {
                if (getSn_SR(socket_index) != SOCK_UDP)                         // 如果socket状态不是SOCK_UDP，则跳出循环
                {
                    printf("socket %d 意外关闭\r\n", socket_index);
                    close(socket_index);
                }
                
            }
            setSn_IR(socket_index, Sn_IR_RECV);                                 // 清除接收标志
            
            // 对于UDP而言，接收的数据长度比实际接收的数据要大，因为UDP协议在数据前面加了包头，包头长度为8字节，所以要减去8字节
            length = getSn_RX_RSR(socket_index);                                // 获取接收数据的长度
            if (length > 8)
            {
                memset(g_w5500_data_buff, 0, length + 1);
                recvfrom(socket_index, g_w5500_data_buff, length - 8, targetIP, &targetPort);   // 接收数据
                printf("%d.%d.%d.%d:%d 发来数据：%s\r\n", targetIP[0], targetIP[1], targetIP[2], targetIP[3], targetPort, g_w5500_data_buff);
                sendto(socket_index, g_w5500_data_buff, length - 8, targetIP, targetPort);      // 发送回对方
            }
        }

    default:
        break;
    }
}

/**
 * @brief UDP发送数据
 * 
 * @param socket_index socket索引
 * @param data 发送数据缓冲区
 * @param length 发送数据长度
 * @param targetIP 发送目标IP
 * @param targetPort 发送目标端口
 */
void UDP_SendData(uint8_t socket_index, uint8_t *data, uint16_t length, uint8_t *targetIP, uint16_t targetPort)
{                                 
    if (getSn_SR(socket_index) == SOCK_UDP)                                     // 获取Socket状态
    {
        sendto(socket_index, data, length, targetIP, targetPort);               // 发送回对方
    }
}

/**
 * @brief UDP接收数据
 * 
 * @param socket_index socket索引
 * @param data 保存接收数据的缓冲区
 * @param length 接收数据长度
 * @param targetIP 发送方IP
 * @param targetPort 发送方端口
 */
void UDP_ReceiveData(uint8_t socket_index, uint8_t *data, uint16_t *length, uint8_t *targetIP, uint16_t *targetPort)
{                           
    if (getSn_SR(socket_index) == SOCK_UDP)                                     // 获取Socket状态
    {
        if ((getSn_IR(socket_index) & Sn_IR_RECV))                              // 根据事件标志判断是否接收到数据
        {
            setSn_IR(socket_index, Sn_IR_RECV);                                 // 清除中断标志位，写1清除，写0无效
            *length = getSn_RX_RSR(socket_index);                               // 获取接收到数据长度
            if (*length > 8)
            {
                recvfrom(socket_index, data, *length - 8, targetIP, targetPort);    // 接收数据
            }
        }
    }
}