#include "eeprom.h"

/**  根据不同的 24CXX 型号, 发送高位地址
 * 1、24C16以上的型号，分 2 个字节发送地址
 * 2、24C16及以下的型号,，分 1 个低字节地址 + 占用器件地址的 bit1~bit3 位用于表示高位地址，最多 11 位地址
 *    对于 24C01/02，其器件地址格式（8bit）为: 1  0  1  0  A2  A1  A0  R/W
 *    对于 24C04，   其器件地址格式（8bit）为: 1  0  1  0  A2  A1  a8  R/W
 *    对于 24C08，   其器件地址格式（8bit）为: 1  0  1  0  A2  a9  a8  R/W
 *    对于 24C16，   其器件地址格式（8bit）为: 1  0  1  0  a10 a9  a8  R/W
 *    R/W      : 读/写控制位 0: 表示写; 1: 表示读;
 *    A0/A1/A2 : 对应器件的 1、2、3引脚（只有 24C01/02/04/8 有这些脚）
 *    a8/a9/a10: 对应存储整列的高位地址，11bit 地址最多可以表示 2048 个位置, 可以寻址 24C16 及以内的型号
 */  

static void EEPROM_WriteOnePage(uint16_t address, uint8_t *pBuffer, uint8_t length);

/**
 * @brief EEPROM读一个字节函数
 * 
 * @param address 待读取数据的内存地址
 * @return uint8_t 读取的数据
 */
uint8_t EEPROM_ReadOneByte(uint16_t address)
{
    uint8_t receive = 0;

    BSP_Simulate_I2C_Start();                                                   // 1、产生起始信号

    if (EEPROM_TYPE <= AT24C16)
    {
        BSP_Simulate_I2C_SendOneByte((EEPROM_ADDRESS << 1) + ((address >> 8) << 1));    // 2、发送器件地址 + 高位a8/a9/a10地址 + 写操作，写数据
    }
    else if (EEPROM_TYPE > AT24C16 && EEPROM_TYPE <= AT24C512)                  // AT24C16以上AT24C512以下的型号，分2个字节发送地址
    {
        BSP_Simulate_I2C_SendOneByte(EEPROM_ADDRESS << 1);                      // 2、发送写操作地址
        BSP_Simulate_I2C_WaitAck();                                             // 3、等待应答信号
        BSP_Simulate_I2C_SendOneByte(address >> 8);                             // 4、发送高字节地址
    }
    BSP_Simulate_I2C_WaitAck();                                                 // 每次发送完一个字节,都要等待ACK
    BSP_Simulate_I2C_SendOneByte(address % 256);                                // 4、发送内存地址（低地址）
    BSP_Simulate_I2C_WaitAck();                                                 // 5、等待应答信号
    BSP_Simulate_I2C_Start();                                                   // 6、再次产生起始信号
    BSP_Simulate_I2C_SendOneByte(EEPROM_ADDRESS << 1 | 0x01);                   // 7、发送读操作地址
    BSP_Simulate_I2C_WaitAck();                                                 // 8、等待应答信号
    receive = BSP_Simulate_I2C_ReadOneByte(1);                                  // 9、读取数据，并发送非应答信号
    BSP_Simulate_I2C_Stop();                                                    // 10、产生停止信号

    return receive;                                                             // 11、返回读取的数据
}

/**
 * @brief EEPROM顺序读取多个字节数据函数
 * 
 * @param address 起始地址
 * @param pBuffer 保存读取的数据的缓冲区
 * @param length 读取字节的个数
 */
void EEPROM_ReadBytes(uint16_t address, uint8_t *pBuffer, uint16_t length)
{
    BSP_Simulate_I2C_Start();                                                   // 1、产生起始信号

    if (EEPROM_TYPE <= AT24C16)
    {
        BSP_Simulate_I2C_SendOneByte((EEPROM_ADDRESS << 1) + ((address >> 8) << 1));    // 2、发送器件地址 + 高位a8/a9/a10地址 + 写操作，写数据
    }
    else if (EEPROM_TYPE > AT24C16 && EEPROM_TYPE <= AT24C512)                  // AT24C16以上AT24C512以下的型号，分2个字节发送地址
    {
        BSP_Simulate_I2C_SendOneByte(EEPROM_ADDRESS << 1);                      // 2、发送写操作地址
        BSP_Simulate_I2C_WaitAck();                                             // 3、等待应答信号
        BSP_Simulate_I2C_SendOneByte(address >> 8);                             // 4、发送高字节地址
    }
    BSP_Simulate_I2C_WaitAck();                                                 // 每次发送完一个字节,都要等待ACK
    BSP_Simulate_I2C_SendOneByte(address % 256);                                // 4、发送内存地址（低地址）
    BSP_Simulate_I2C_WaitAck();                                                 // 5、等待应答信号
    BSP_Simulate_I2C_Start();                                                   // 6、再次产生起始信号
    BSP_Simulate_I2C_SendOneByte(EEPROM_ADDRESS << 1 | 0x01);                   // 7、发送读操作地址
    BSP_Simulate_I2C_WaitAck();                                                 // 8、等待应答信号
    for (uint8_t i = 0; i < length; i++)
    {
        pBuffer[i] = BSP_Simulate_I2C_ReadOneByte((i == length - 1) ? 1 : 0);   // 9、读取数据，并发送应答信号，最后一个发送非应答信号
    }
  
    BSP_Simulate_I2C_Stop();                                                    // 10、产生停止信号
}

/**
 * @brief EEPROM写一个字节函数
 * 
 * @param address 待写入数据的内存地址
 * @param data 待写入的数据
 */
void EEPROM_WriteOneByte(uint16_t address, uint8_t data)
{
    BSP_Simulate_I2C_Start();                                                   // 1、产生起始信号

    if (EEPROM_TYPE <= AT24C16)
    {
        BSP_Simulate_I2C_SendOneByte((EEPROM_ADDRESS << 1) + ((address >> 8) << 1));    // 2、发送器件地址 + 高位a8/a9/a10地址 + 写操作，写数据
    }
    else if (EEPROM_TYPE > AT24C16 && EEPROM_TYPE <= AT24C512)                  // AT24C16以上AT24C512以下的型号，分2个字节发送地址
    {
        BSP_Simulate_I2C_SendOneByte(EEPROM_ADDRESS << 1);                      // 2、发送写操作地址
        BSP_Simulate_I2C_WaitAck();                                             // 3、等待应答信号
        BSP_Simulate_I2C_SendOneByte(address >> 8);                             // 4、发送高字节地址
    }
    BSP_Simulate_I2C_WaitAck();                                                 // 每次发送完一个字节,都要等待ACK
    BSP_Simulate_I2C_SendOneByte(address % 256);                                // 4、发送内存地址（低地址）
    BSP_Simulate_I2C_WaitAck();                                                 // 5、等待应答信号
    BSP_Simulate_I2C_SendOneByte(data);                                         // 6、发送数据
    BSP_Simulate_I2C_WaitAck();                                                 // 7、等待应答信号
    BSP_Simulate_I2C_Stop();                                                    // 8、产生停止信号

    Delay_ms(10);                                                               // 9、等待EEPROM写入完成
}

/**
 * @brief EEPROM在一页中写入多个字节
 * 
 * @param address 待写入数据的内存地址
 * @param pBuffer 待写入的数据
 * @param length 待写入数据的个数
 * 
 * @note EEPROM的页写模式存在页的限制，超过一页（8字节）时，超出数据覆盖原先写入数据
 */
static void EEPROM_WriteOnePage(uint16_t address, uint8_t *pBuffer, uint8_t length)
{
    BSP_Simulate_I2C_Start();                                                   // 1、产生起始信号
    if (EEPROM_TYPE <= AT24C16)
    {
        BSP_Simulate_I2C_SendOneByte((EEPROM_ADDRESS << 1) + ((address >> 8) << 1));    // 2、发送发送器件 0xA0 + 高位a8/a9/a10地址，写数据
    }
    else if (EEPROM_TYPE > AT24C16 && EEPROM_TYPE <= AT24C512)                  // AT24C16以上AT24C512以下的型号，分2个字节发送地址
    {
        BSP_Simulate_I2C_SendOneByte(EEPROM_ADDRESS << 1);                      // 2、发送写操作地址
        BSP_Simulate_I2C_WaitAck();                                             // 3、等待应答信号
        BSP_Simulate_I2C_SendOneByte(address >> 8);                             // 4、发送高字节地址
    }
    BSP_Simulate_I2C_WaitAck();                                                 // 每次发送完一个字节,都要等待ACK
    BSP_Simulate_I2C_SendOneByte(address % 256);                                // 4、发送内存地址（低地址）
    BSP_Simulate_I2C_WaitAck();                                                 // 5、等待应答信号
    for (uint8_t i = 0; i < length; i++)
    {   
        BSP_Simulate_I2C_SendOneByte(pBuffer[i]);                               // 6、发送数据
        BSP_Simulate_I2C_WaitAck();                                             // 7、等待应答信号
    }
    BSP_Simulate_I2C_Stop();                                                    // 8、产生停止信号

    Delay_ms(10);                                                               // 9、等待AT24C02写入完成
}

/**
 * @brief EEPROM写数据函数
 * 
 * @param address 待写入数据的内存地址
 * @param pBuffer 待写入的数据
 * @param length 待写入数据的个数
 */
void EEPROM_WriteBytes(uint16_t address, uint8_t *pBuffer, uint16_t length)
{
    uint16_t offset = address % 8;                                              // 在第一页中的偏移地址
    uint16_t remain = 8 - offset;                                               // 一页剩余空间大小
    uint16_t pageNum = (length - remain) / 8;                                   // 去除第一页后还需要写入的整页数
    uint8_t lastPageByteNum = (length - remain) % 8;                            // 最后一页需要写入的字节数

    // 当写入的数据小一页剩余空间大小，将写入的字节数赋值给页中剩余空间大小
    remain = (length <= remain ? length : remain);

    EEPROM_WriteOnePage(address, pBuffer, remain);                              // 写入第一页数据
    address += remain;
    pBuffer += remain;
    
    while (pageNum--)
    {
        EEPROM_WriteOnePage(address, pBuffer, 8);                               // 写入整页数据
        address += 8;
        pBuffer += 8;
    }
    
    if (lastPageByteNum)
    {
        EEPROM_WriteOnePage(address, pBuffer, lastPageByteNum);                // 写入最后一页数据
    }
}