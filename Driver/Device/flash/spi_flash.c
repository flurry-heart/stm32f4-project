#include "spi_flash.h"

uint8_t g_SPI_FLASH_buffer[4096];

static uint8_t SPI_FLASH_ReadAndWriteOneByte(uint8_t data);
static void SPI_FLASH_SendAddress(uint32_t address);
static void SPI_FLASH_WriteEnable(void);
static void SPI_FLASH_WaitBusy(void);
static uint8_t SPI_FLASH_ReadRegisterSR(uint8_t regno);
static void SPI_FLASH_WriteRegisterSR(uint8_t regno, uint8_t data);
static void SPI_FLASH_PageProgram(uint32_t address, uint8_t *data, uint16_t length);
static void SPI_FLASH_WriteData_NoCheck(uint32_t address, uint8_t *data, uint16_t length);

/**
 * @brief SPI FLASH初始化函数
 * 
 */
void SPI_FLASH_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    RCC_SPI_FLASH_CS_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = SPI_FLASH_CS_GPIO_PIN;                                // SPI_FLASH的CS引脚
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;                                 // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;                                         // 不使用上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                               // 输出速度
    HAL_GPIO_Init(SPI_FLASH_CS_GPIO_PORT, &GPIO_InitStruct);

    SPI_FLASH_CS(1);                                                            // 片选引脚默认为高电平，不选中从机
    
    if (W25Qxxx == W25Q256)                                                     // SPI_FLASH为W25Q256，必须使能4字节地址模式
    {
        int temp = SPI_FLASH_ReadRegisterSR(3);                                 // 读取状态寄存器3，判断地址模式

        if ((temp & 0x01) == 0)                                                 // 如果不是4字节地址模式，则进入4字节地址模式
        {
            SPI_FLASH_WriteEnable();                                            // 写使能
            temp |= (1 << 1);                                                   //  ADP=1，上电4位地址模式 
            SPI_FLASH_WriteRegisterSR(3, temp);                                 // 写SR3
            
            SPI_FLASH_CS(0);
            SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_ENTER_4BYTE_ADDRESS_MODE);  // 使能4字节地址指令
            SPI_FLASH_CS(1);
        }
    }
}

/**
 * @brief 往SPI FLASH读写一个字节
 * 
 * @param data 写入的一个字节数数据
 * @return uint8_t 读取的一个字节的数据
 */
static uint8_t SPI_FLASH_ReadAndWriteOneByte(uint8_t data)
{
    return BSP_Simulate_SPI_SwapOneByte(data);
}

/**
 * @brief SPI FLASH发送地址函数
 * 
 * @param address 内存地址
 */
static void SPI_FLASH_SendAddress(uint32_t address)
{
    if (W25Qxxx == W25Q256)                                                     // 只有W25Q256支持4字节地址模式
    {
        SPI_FLASH_ReadAndWriteOneByte((uint8_t)((address) >> 24));              // 发送 bit31 ~ bit24 地址
    } 
    SPI_FLASH_ReadAndWriteOneByte((uint8_t)((address) >> 16));                  // 发送 bit23 ~ bit16 地址
    SPI_FLASH_ReadAndWriteOneByte((uint8_t)((address) >> 8));                   // 发送 bit15 ~ bit8  地址
    SPI_FLASH_ReadAndWriteOneByte((uint8_t)address);                            // 发送 bit7 ~ bit0  地址
}

/**
 * @brief 读取SPI FLASH的状态寄存器函数
 * 
 * @param regno 寄存器编号，范围: 1~3
 * @return uint8_t 寄存器的值
 * 
 * @note
 *  状态寄存器1：
 *      BIT7  6   5   4   3   2   1   0
 *      SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *      SPR: 默认0，状态寄存器保护位，配合WP使用
 *      TB,BP2,BP1,BP0: SPI FLASH区域写保护设置
 *      WEL: 写使能锁定
 *      BUSY: 忙标记位(1: 忙; 0: 空闲)
 *      默认: 0x00
 *
 *  状态寄存器2：
 *      BIT7  6   5   4   3   2   1   0
 *      SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 *
 *  状态寄存器3：
 *      BIT7      6    5    4   3   2   1   0
 *      HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 */
static uint8_t SPI_FLASH_ReadRegisterSR(uint8_t regno)
{
    uint8_t byte = 0, command = 0;

    switch (regno)
    {
        case 1:
            command = SPI_FLASH_READ_STATUS_REGISTER1;                          // 读状态寄存器1指令
            break;

        case 2:
            command = SPI_FLASH_READ_STATUS_REGISTER2;                          // 读状态寄存器2指令
            break;

        case 3:
            command = SPI_FLASH_READ_STATUS_REGISTER3;                          // 读状态寄存器3指令
            break;

        default:
            command = SPI_FLASH_READ_STATUS_REGISTER1;
            break;
    }

    SPI_FLASH_CS(0);
    SPI_FLASH_ReadAndWriteOneByte(command);                                     // 发送读寄存器命令
    byte = SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_DUMMY_BYTE);                 // 读取一个字节
    SPI_FLASH_CS(1);
    
    return byte;
}

/**
 * @brief 写状态寄存器函数
 * 
 * @param regno 状态寄存器的编号，范围: 1~3
 * @param data 要写入状态寄存器的值
 */
static void SPI_FLASH_WriteRegisterSR(uint8_t regno, uint8_t data)
{
    uint8_t command = 0;

    switch (regno)
    {
        case 1:
            command = SPI_FLASH_WRITE_STATUS_REGISTER1;                         // 写状态寄存器1指令
            break;

        case 2:
            command = SPI_FLASH_WRITE_STATUS_REGISTER2;                         // 写状态寄存器2指令
            break;

        case 3:
            command = SPI_FLASH_WRITE_STATUS_REGISTER3;                         // 写状态寄存器3指令
            break;

        default:
            command = SPI_FLASH_WRITE_STATUS_REGISTER1;
            break;
    }

    SPI_FLASH_CS(0);
    SPI_FLASH_ReadAndWriteOneByte(command);                                     // 发送读寄存器命令
    SPI_FLASH_ReadAndWriteOneByte(data);                                        // 写入一个字节
    SPI_FLASH_CS(1);
}

/**
 * @brief SPI FLASH读数据函数
 * 
 * @param address 待写入数据的内存地址
 * @param data 指向数据指针
 * @param length 待写入的数据长度
 */
void SPI_FLASH_ReadData(uint32_t address, uint8_t *data, uint16_t length)
{
    // 拉低片选
    SPI_FLASH_CS(0);

    // 发送读数据命令
    SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_READ_DATA);

    // 发送扇区地址
    SPI_FLASH_SendAddress(address);

    // 读取数据
    for (uint32_t i = 0; i < length; i++)
    {
        data[i] = SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_DUMMY_BYTE);
    }

    // 拉高片选
    SPI_FLASH_CS(1);
}

/**
 * @brief SPI FLASH写使能函数
 * 
 */
static void SPI_FLASH_WriteEnable(void)
{
    // 拉低片选
    SPI_FLASH_CS(0);
    // 发送写使能命令
    SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_WRITE_ENABLE);
    // 拉高片选
    SPI_FLASH_CS(1);
}

/**
 * @brief SPI FLASH等待完成函数
 * 
 */
static void SPI_FLASH_WaitBusy(void)
{
    uint32_t time = 0xFFFF;

    // 拉低片选
    SPI_FLASH_CS(0);
    // 发送读状态寄存器1命令
    SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_READ_STATUS_REGISTER1);
    // 等待写使能完成
    while((SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_DUMMY_BYTE) & 0x01) || time--);
    // 拉高片选
    SPI_FLASH_CS(1);
}

/**
 * @brief SPI_FLASH扇区擦除函数
 * 
 * @param address 待删除的扇区的内存地址
 */
void SPI_FLASH_SectorErase(uint32_t address)
{
    // 写使能
    SPI_FLASH_WriteEnable();

    // 拉低片选
    SPI_FLASH_CS(0);

    // 发送扇区擦除命令
    SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_SECTOR_ERASE_4KB);

    // 发送扇区地址
    SPI_FLASH_SendAddress(address);

    // 拉高片选
    SPI_FLASH_CS(1);

    // 等待擦除完成
    SPI_FLASH_WaitBusy();
}

/**
 * @brief SPI FLASH页写入函数
 * 
 * @param address 待写入数据的内存地址
 * @param data 待写入的数据
 * @param length 待写入数据的长度
 */
static void SPI_FLASH_PageProgram(uint32_t address, uint8_t *data, uint16_t length)
{
    // 写使能
    SPI_FLASH_WriteEnable();

    // 拉低片选
    SPI_FLASH_CS(0);

    // 发送页写入命令
    SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_PAGE_PROGRAM);

    // 发送扇区地址
    SPI_FLASH_SendAddress(address);

    // 发送数据
    for (uint16_t i = 0; i < length; i++)
    {
        SPI_FLASH_ReadAndWriteOneByte(data[i]);
    }

    // 拉高片选
    SPI_FLASH_CS(1);

    // 等待写使能完成
    SPI_FLASH_WaitBusy();
}

/**
 * @brief 无校验写SPI FLASH函数
 * 
 * @param address 待写入数据的内存地址
 * @param data 待写入的数据
 * @param length 待写入数据的个数
 * 
 * @note 
 *      确保所写地址范围的数据全为0xFF，否则在非0xFF处写入失败。
 *      该函数具有自动换页的功能
 */
static void SPI_FLASH_WriteData_NoCheck(uint32_t address, uint8_t *data, uint16_t length)
{
    uint16_t pageRemain = 256 - address % 256;                                  // 单页剩余的字节数，得到地址在某页的位置

    // 当写入的数据小于单页剩余的字节数，将写入的字节数赋值给单页剩余的字节数
    pageRemain =  (length <= pageRemain ? length : pageRemain);
  
    while (1)
    {
        // 当写入字节比页内剩余地址还少的时候，一次性写完
        // 当写入字节比页内剩余地址多的时候，先写完页内剩余地址，然后根据剩余长度进行不同的处理
        SPI_FLASH_PageProgram(address, data, pageRemain);

  
        if (length == pageRemain)                                               // 写入完成
        {
            break;
        }
        else
        {
            address += pageRemain;                                              // 写地址偏移
            data += pageRemain;                                                 // 写数据指针偏移
            length -= pageRemain;                                               // 写入总长度减去已经写入的个数

            // 当剩余长度大于一页256时，可以一次写一页
            // 当剩余数据小于一页，可以一次写完
            pageRemain = (length > 256 ? 256 : length);
        }
    }
}

/**
 * @brief SPI FLASH写数据函数
 * 
 * @param address 待写入数据的内存地址
 * @param data 待写入的数据
 * @param length 待写入数据的个数
 */
void SPI_FLASH_WriteData(uint32_t address, uint8_t *data, uint16_t length)
{
    uint32_t sectorPosition = address / 4096;                                   // 扇区地址
    uint16_t sectorOffset = address % 4096;                                     // 在扇区中的偏移地址
    uint16_t sectorRemain = 4096 - sectorOffset;                                // 扇区剩余空间大小
    uint8_t *pBuff = g_SPI_FLASH_buffer;
    uint16_t i = 0;

    // 当写入的数据小扇区剩余空间大小，将写入的字节数赋值给扇区剩余空间大小
    sectorRemain = (length <= sectorRemain ? length : sectorRemain);
  
    while (1)
    {
        // 读出整个扇区的内容
        SPI_FLASH_ReadData(sectorPosition * 4096, pBuff, 4096);

        // 校验数据是否需要擦除
        for (i = 0; i < sectorRemain; i++)
        {
            if (pBuff[sectorOffset + i] != 0xFF)
            {
                break;                                                          // 需要擦除，直接退出for循环
            }
        }

        // 需要擦除
        if (i < sectorRemain)
        {
            // 擦除这个扇区
            SPI_FLASH_SectorErase(sectorPosition * 4096);

            // 将待写入的数据拷贝到缓冲区
            for (i = 0; i < sectorRemain; i++)
            {
                pBuff[i + sectorOffset] = data[i];
            }
            //写入整个扇区
            SPI_FLASH_WriteData_NoCheck(sectorPosition * 4096, pBuff, 4096);
        }
        else
        {
            // 对于已经擦除的，直接写入扇区剩余空间
            SPI_FLASH_WriteData_NoCheck(address, data, sectorRemain);
        }
  
        // 写入完成
        if (length == sectorRemain)
        {
            break;
        }
        // 写入未完成
        else
        {
            sectorPosition++;                                                   // 扇区加1，使用下一个扇区
            sectorOffset = 0;                                                   // 扇区偏移地址为0

            address += sectorRemain;                                            // 写地址偏移
            data += sectorRemain;                                               // 写数据指针偏移
            length -= sectorRemain;                                             // 写入总长度减去已经写入的个数

            // 当剩余长度大于扇区长度4096时，可以一次写一整个扇区
            // 当剩余长度小于扇区长度4096时，下一个扇区可以写完
            sectorRemain = (length > 4096 ? 4096 : length);
        }
    }
}

/**
 * @brief SPI FLASH读取ID函数
 * 
 * @return uint32_t SPI_FLASH的ID
 */
uint32_t SPI_FLASH_ReadID(void)
{
    uint8_t temp[3] = {0};
    uint32_t id = 0;

    // 拉低片选
    SPI_FLASH_CS(0);

    // 发送读ID命令
    SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_JEDEC_ID);

    // 读取ID
    for (uint8_t i = 0; i < 3; i++)
    {
        temp[i] = SPI_FLASH_ReadAndWriteOneByte(SPI_FLASH_DUMMY_BYTE);
        id <<= 8;
        id |= temp[i];
    }

    // 拉高片选
    SPI_FLASH_CS(1);

    // 返回ID
    return id;
}