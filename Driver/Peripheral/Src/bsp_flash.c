#include "bsp_flash.h"

static uint32_t BSP_FLASH_ReadOneWord(uint32_t address);
static uint8_t  BSP_FLASH_GetFlashSector(uint32_t address);

/**
 * @brief 内部FLASH读取一个字节函数
 * 
 * @param address 待读取数据的地址
 * @return uint32_t 读取的数据
 */
static uint32_t BSP_FLASH_ReadOneWord(uint32_t address)
{
    return *(volatile uint32_t *)address;
}

/**
 * @brief 获取某个地址所在的FLASH扇区
 * 
 * @param address 内存地址
 * @return uint8_t address所在的扇区
 */
static uint8_t BSP_FLASH_GetFlashSector(uint32_t address)
{
    if (address < ADDRESS_FLASH_SECTOR_1) return FLASH_SECTOR_0;
    else if (address < ADDRESS_FLASH_SECTOR_2) return FLASH_SECTOR_1;
    else if (address < ADDRESS_FLASH_SECTOR_3) return FLASH_SECTOR_2;
    else if (address < ADDRESS_FLASH_SECTOR_4) return FLASH_SECTOR_3;
    else if (address < ADDRESS_FLASH_SECTOR_5) return FLASH_SECTOR_4;
    else if (address < ADDRESS_FLASH_SECTOR_6) return FLASH_SECTOR_5;
    else if (address < ADDRESS_FLASH_SECTOR_7) return FLASH_SECTOR_6;
    else if (address < ADDRESS_FLASH_SECTOR_8) return FLASH_SECTOR_7;
    else if (address < ADDRESS_FLASH_SECTOR_9) return FLASH_SECTOR_8;
    else if (address < ADDRESS_FLASH_SECTOR_10) return FLASH_SECTOR_9;
    else if (address < ADDRESS_FLASH_SECTOR_11) return FLASH_SECTOR_10;
    return FLASH_SECTOR_11;
}

/**
 * @brief 从指定地址开始读出指定长度的数据
 * 
 * @param address 起始地址
 * @param data 存放读取数据缓冲区的指针
 * @param length 要读取的字（32位）数，即4个字节的整数倍
 */
void BSP_FLASH_ReadData(uint32_t address, uint32_t *data, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        data[i] = BSP_FLASH_ReadOneWord(address);                               // 读取一个字，四个字节
        address += 4;                                                           // 地址偏移4个字节
    }
}

/**
 * @brief 在FLASH指定位置，写入指定长度的数据（自动擦除）函数
 * 
 * @param address 起始地址（此地址必须为4的倍数，否则写入出错！）
 * @param data 数据指针
 * @param length 要写入的字（32位）的个数
 * 
 * @note    因为STM32F4的扇区实在太大，没办法本地保存扇区数据，所以本函数写地址如果非0XFF，
 *          那么会先擦除整个扇区且不保存扇区数据.所以写非0XFF的地址，将导致整个扇区数据丢失。
 *          建议写之前确保扇区里没有重要数据，最好是整个扇区先擦除了，然后慢慢往后写。
 *          该函数对OTP区域也有效！可以用来写OTP区！
 *          OTP区域地址范围：0X1FFF7800~0X1FFF7A0F（注意：最后16字节，用于OTP数据块锁定！）
 */
void BSP_FLASH_WriteData(uint32_t address, uint32_t *data, uint32_t length)
{
    FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {0};
    HAL_StatusTypeDef flash_status = HAL_OK;

    uint32_t start_address = 0;
    uint32_t end_address = 0;
    uint32_t sectorerror=0;

    // 写入地址小于或大于FLASH地址，或不是4的整数倍，非法
    if (address < FLASH_BASE || (address % 4) || (address > FLASH_END))
    {
        return;
    }

    HAL_FLASH_Unlock();                                                         // 解锁
    FLASH->ACR &= ~(1 << 10);                                                   // FLASH擦除期间，必须禁止数据缓存

    start_address = address;                                                    // 写入的起始地址
    end_address = address + length * 4;                                         // 写入的结束地址

    if (start_address < 0x1FFF0000)                                             // 只有主存储区,才需要执行擦除操作
    {
        while (start_address < end_address)                                     // 对非0xFFFFFFFF的地方,先擦除
        {
            if (BSP_FLASH_ReadOneWord(start_address) != 0xFFFFFFFF)             // 有非0xFFFFFFFF的地方，要擦除这个扇区
            {
                FLASH_EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;      // 擦除类型，扇区擦除
                FLASH_EraseInitStruct.Sector = BSP_FLASH_GetFlashSector(start_address); // 要擦除的扇区
                FLASH_EraseInitStruct.NbSectors = 1;                            // 一次只擦除一个扇区
                FLASH_EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;     // 电压范围，VCC=2.7~3.6V之间!

                if(HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &sectorerror) != HAL_OK) 
                {
                    break;                                                      // 发生错误了退出
                }
            }
            else
            {
                start_address += 4;                                             // 地址偏移4个字节
            }
            FLASH_WaitForLastOperation(FLASH_WAITE_TIME);                       // 等待上次操作完成
        }
    }

    flash_status = FLASH_WaitForLastOperation(FLASH_WAITE_TIME);                // 等待上次操作完成

    if (flash_status == HAL_OK)
    {
        while (address < end_address)                                           // 写数据
        {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *data) != HAL_OK)    // 写入数据
            {
                break;                                                          // 写入异常则退出
            }

            address += 4;                                                       // 地址偏移4个字节
            data++;                                                             // 数据指针后移
        }
    }

    FLASH->ACR |= 1 << 10;                                                      // FLASH擦除结束，开启数据fetch

    HAL_FLASH_Lock();                                                           // 上锁
}