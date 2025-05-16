#include "w5500_device.h"

SPI_HandleTypeDef g_w5500_spi_handle;

wiz_NetInfo g_w5500_net_info = 
{
    .mac  = {0x00, 0x08, 0xdc, 0x12, 0x22, 0x12},                               // MAC地址，一共6个字节，别与别人的冲突
    .ip   = {192, 168, 3, 210},                                                 //  IP地址，一共4个字节，前三段与电脑一致，处于同一个网段中，最后一段注意不要与别人冲突
    .gw   = {192, 168, 1, 1},                                                   // 网关
    .sn   = {255, 255, 255, 0},                                                 // 子网掩码
    .dns  = {144, 144, 144, 144},                                               // DNS服务器
    .dhcp = NETINFO_STATIC                                                      // 是否使用DHCP
};

uint8_t g_w5500_data_buff[DATA_BUFFER_SIZE];                                    // 数据缓冲区

/**
 * @brief W5500初始化函数
 * 
 * @param hspi SPI句柄
 */
void W5500_Init(SPI_HandleTypeDef *hspi)
{
    g_w5500_spi_handle = *hspi;
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    RCC_W5500_CS_GPIO_CLK_ENABLE();
    RCC_W5500_RESET_GPIO_CLK_ENABLE();
    RCC_W5500_INTERRUPT_GPIO_CLK_ENABLE();

    // 推挽输出
    GPIO_InitStruct.Pin = W5500_CS_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(W5500_CS_GPIO_PORT, &GPIO_InitStruct);

    // 推挽输出
    GPIO_InitStruct.Pin = W5500_RESET_GPIO_PIN;
    HAL_GPIO_Init(W5500_RESET_GPIO_PORT, &GPIO_InitStruct);

    // 上拉输入
    GPIO_InitStruct.Pin = W5500_INTERRUPT_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(W5500_INTERRUPT_GPIO_PORT, &GPIO_InitStruct);

    W5500_Reset();                                                              // 重启芯片

    register_wizchip_function();                                                // 调用注册函数
}

/**
 * @brief W5500复位
 * 
 */
void W5500_Reset(void)
{
    W5500_RESET(0);
    // 拉低复位引脚最低500ms可以使芯片重启
    HAL_Delay(100);
    W5500_RESET(1);
    HAL_Delay(300);
}

/**
 * @brief W5500设置MAC地址
 * 
 */
void W5500_SetMac(void)
{
    setSHAR(g_w5500_net_info.mac);                                              // 设置MAC地址
    printf("设置MAC地址成功\r\n");
}

/**
 * @brief W5500设置IP地址
 * 
 */
void W5500_SetIp(void)
{
    setSIPR(g_w5500_net_info.ip);                                               // 设置IP地址
    setSUBR(g_w5500_net_info.sn);                                               // 设置子网掩码
    setGAR(g_w5500_net_info.gw);                                                // 设置网关
    printf("设置IP地址成功\r\n");
}

/**
 * @brief 打印网络信息函数
 * 
 */
void PrintInfo(void)
{
    uint8_t info[6] = {0};
    wiz_NetInfo netInfo;

    ctlnetwork(CN_GET_NETINFO, (void*)&netInfo);                                // 获取网络信息
    ctlwizchip(CW_GET_ID,(void*)info);                                          // 显示网络信息

    if(netInfo.dhcp == NETINFO_DHCP) 
    {
        printf("\r\n=== %s NET CONF : DHCP ===\r\n",(char*)info);
    }
    else 
    {
        printf("\r\n=== %s NET CONF : Static ===\r\n",(char*)info);
    }

    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", netInfo.mac[0], netInfo.mac[1], netInfo.mac[2], netInfo.mac[3], netInfo.mac[4], netInfo.mac[5]);
    printf("SIP: %d.%d.%d.%d\r\n", netInfo.ip[0], netInfo.ip[1], netInfo.ip[2], netInfo.ip[3]);
    printf("GAR: %d.%d.%d.%d\r\n", netInfo.gw[0], netInfo.gw[1], netInfo.gw[2], netInfo.gw[3]);
    printf("SUB: %d.%d.%d.%d\r\n", netInfo.sn[0], netInfo.sn[1], netInfo.sn[2], netInfo.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", netInfo.dns[0], netInfo.dns[1], netInfo.dns[2], netInfo.dns[3]);
    printf("===========================\r\n");
}

/**
 * @brief DHCP分配IP地址
 * 
 * @param socket_index 端口号
 */
void DHCP_TryToGetIp(uint8_t socket_index)
{
    static uint8_t retry_count = 0;
    uint8_t state = 0;

    memset(g_w5500_data_buff, 0, DATA_BUFFER_SIZE);

    g_w5500_net_info.dhcp = NETINFO_DHCP;                                       // 设置为DHCP模式

    setSHAR(g_w5500_net_info.mac);                                              // 在DPCH开始之前需要手动设置MAC地址

    // 注册分配IP时，IP更新时，IP冲突时的回调函数，如果参数为NULL，则使用默认的注册函数
    reg_dhcp_cbfunc(NULL, NULL, NULL);  

    DHCP_init(socket_index, g_w5500_data_buff);                                 // 初始化DHCP

    printf("DHCP开始分配IP\r\n");

    // 返回DHCP_IP_LEASED说明申请租赁IP地址成功，如果没成功，重新申请
    state = DHCP_run();

    while(state != DHCP_IP_LEASED && retry_count < DHCP_MAX_RETRY_COUNT)
    {
        retry_count++;
        printf("DHCP第%d次分配IP失败\r\n", retry_count);
        state = DHCP_run();
        Delay_ms(1000);
    }
    
    if (state != DHCP_IP_LEASED && retry_count == DHCP_MAX_RETRY_COUNT)
    {
        retry_count = 0;
        printf("DHCP分配IP失败，采用静态分配IP的方式\r\n");
        g_w5500_net_info.dhcp = NETINFO_STATIC;
    }

    DHCP_stop();

    getIPfromDHCP(g_w5500_net_info.ip);
    getGWfromDHCP(g_w5500_net_info.gw);
    getSNfromDHCP(g_w5500_net_info.sn);
    getDNSfromDHCP(g_w5500_net_info.dns);

    ctlnetwork(CN_SET_NETINFO, (void*)&g_w5500_net_info);                       // 设置网络信息

    close(socket_index);                                                        // 关闭端口
}