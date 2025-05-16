#include "mqtt.h"

/**
 * @brief 构建MQTT连接包
 * 
 * @param mqtt_message 保存构建的MQTT连接包
 * @param client_id 客户端id
 * @param username 用户名
 * @param password 密码
 * @return uint16_t 构建的MQTT连接报文长度
 * 
 * @note MQTT连接包格式如下：
 *      固定报头: 报文类型 (10) 剩余长度=可变报头+负载 (??)
 *      可变报头：
 *          协议名: 协议名长度 (00 04) MQTT (4D 51 54 54)
 *          协议版本: (04)
 *          连接标志: (C2)
 *          保活时间: (?? ??)
 *      有效载荷：
 *          客户端ID：客户端ID长度 (?? ??) 客户端ID (client_id)
 *          用户名: 用户名长度 (?? ??) 用户名 (username)
 *          密码: 密码长度 (?? ??) 密码 (password)
 */
uint16_t MQTT_ConnectMessage(uint8_t*mqtt_message,char *client_id,char *username,char *password)
{
    uint16_t client_id_length = strlen(client_id);
    uint16_t username_length = strlen(username);
    uint16_t password_length = strlen(password);
    uint16_t remain_length = 0;
    uint16_t index = 0;

    // MQTT连接报文类型
    mqtt_message[index++] = 0x10;                                               // MQTT Message Type CONNECT

    // 剩余长度=可变报头长度（10）+客户端ID长度（2）+客户端ID（client_id_length）
    remain_length = 10 + 2 + client_id_length;
    // 用户名长度（2）+用户名（username_length）
    remain_length = (username_length > 0) ? (remain_length + 2 + username_length) : remain_length;
    // 密码长度（2）+密码（password_length）
    remain_length = (password_length > 0) ? (remain_length + 2 + password_length) : remain_length;

    // 循环处理固定报文中的剩余长度字节，字节量根据剩余字节的真实长度变化
    do {
        int temp = remain_length % 128;                                         // 剩余长度取余
        remain_length = remain_length / 128;                                    // 剩余长度取整
        (remain_length > 0) ? (temp |= 0x80) : temp;                            // 按协议要求位7置位
        mqtt_message[index++] = temp;                                           // 剩余长度字节记录一个数据
    } while (remain_length > 0);                                                // 如果remain_length大于0，再次进入循环

    // 协议名长度：00 04
    mqtt_message[index + 0] = 0x00;                                             // Protocol Name Length MSB 
    mqtt_message[index + 1] = 0x04;                                             // Protocol Name Length LSB 

    // 协议名：MQTT   
    mqtt_message[index + 2] = 0x4D;                                             // ASCII Code for M    
    mqtt_message[index + 3] = 0x51;                                             // ASCII Code for Q    
    mqtt_message[index + 4] = 0x54;                                             // ASCII Code for T    
    mqtt_message[index + 5] = 0x54;                                             // ASCII Code for T 

    // 协议版本
    mqtt_message[index + 6] = 0x04;                                             // MQTT Protocol version = 4   

    //  连接标志
    mqtt_message[index + 7] = 0xC2;                                             // conn flags 

    // 保活时间
    mqtt_message[index + 8] = 0x00;                                             // Keep-alive Time Length MSB    
    mqtt_message[index + 9] = 0x64;                                             // Keep-alive Time Length LSB  

    // 客户端ID长度
    mqtt_message[index + 10] = client_id_length / 256;                          // Client ID length MSB    
    mqtt_message[index + 11] = client_id_length % 256;                          // Client ID length LSB

    // 客户端ID
    for(uint8_t i = 0; i < client_id_length; i++)
    {
        mqtt_message[index + 12 + i] = client_id[i];
    }

    if(username_length > 0)
    {
        // 用户名长度  
        mqtt_message[index + 12 + client_id_length] = username_length / 256;    // username length MSB    
        mqtt_message[index + 13 + client_id_length] = username_length % 256;    // username length LSB

        // 用户名
        for(uint8_t i = 0; i < username_length ; i++)
        {
            mqtt_message[index + 14 + client_id_length + i] = username[i];  
        }
    }

    if(password_length > 0)
    {
        // 密码长度
        mqtt_message[index + 14 + client_id_length + username_length] = password_length / 256;  // password length MSB    
        mqtt_message[index + 15 + client_id_length + username_length] = password_length % 256;  // password length LSB    

        // 密码
        for(uint8_t i = 0; i < password_length; i++)
        {
            mqtt_message[index + 16 + client_id_length + username_length + i] = password[i]; 		
        }
        index = index + 16 + client_id_length + username_length + password_length; 
    }

    return index;
}

/**
 * @brief 通过MQTT向云平台发布信息
 * 
 * @param mqtt_message 保存构建的MQTT发布数据包
 * @param topic 发布主题
 * @param message 发布消息
 * @param udp 重发标志, 0: 表示这是客户端或服务端第一次请求发送这个PUBLISH报文; 1: 表示这可能是一个早前报文请求的重发
 * @param QoS 发布质量, 0: 最多分发一次; 1: 至少分发一次; 2: 只分发一次
 * @param retain 是否保留消息, 0: 不保留 1: 保留
 * @return uint16_t 构建的MQTT推送数据包长度
 * 
 * @note MQTT发送包格式如下：
 *      固定报头：报文类型 (3?) 剩余长度=可变报头+负载 (??)
 *      可变报头：
 *          主题: 主题长度 (?? ??) + 主题 (topic: /sys/{ProductKey}/{deviceName}/thing/event/property/post)
 *          报文标识符: 等级1或等级2有，等级0没有报文标识符
 *      有效载荷：
 *          消息：message (JSON格式数据)
 */
uint16_t MQTT_PublishMessage(uint8_t * mqtt_message, char * topic, char * message, uint8_t udp, uint8_t QoS, uint8_t retain)
{
    static uint16_t id = 0;
    uint16_t topic_length = strlen(topic);
    uint16_t message_length = strlen(message);
    uint16_t index = 0;
    uint16_t remain_length = 0;

    // MQTT发布报文类型
    mqtt_message[index++] = 0x30 | (udp << 3) | (QoS << 2) | (retain);          // MQTT Message Type PUBLISH

    // 剩余长度=可变报头长度（主题名长度（2） + 主题长度（topic_length） + 报文标识符长度（0或2））+ 有效载荷长度（消息长度）
    remain_length = 2 + topic_length + (QoS ? 2 : 0) + message_length;

    // 循环处理固定报文中的剩余长度字节，字节量根据剩余字节的真实长度变化
    do {
        int temp = remain_length % 128;                                         // 剩余长度取余
        remain_length = remain_length / 128;                                    // 剩余长度取整
        (remain_length > 0) ? (temp |= 0x80) : temp;                            // 按协议要求位7置位
        mqtt_message[index++] = temp;                                           // 剩余长度字节记录一个数据
    } while (remain_length > 0);                                                // 如果remain_length大于0，再次进入循环

    // 主题长度
    mqtt_message[index++] = (0xff00 & topic_length) >> 8;
    mqtt_message[index++] = 0xff & topic_length;

    // 主题
    for(uint8_t i = 0; i < topic_length; i++)
    {
        mqtt_message[index + i] = topic[i];
    }
    index += topic_length;

    // 报文标识符，等级0没有
    if(QoS)
    {
        mqtt_message[index++] = (0xff00 & id) >> 8;
        mqtt_message[index++] = 0xff & id;
        id++;
    }

    // 消息
    for(uint8_t i = 0; i < message_length; i++)
    {
        mqtt_message[index + i] = message[i];
    }
    index += message_length;

    return index;
}