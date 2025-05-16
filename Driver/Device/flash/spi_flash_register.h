#ifndef __SPI_FLASH_REGISTER_H__
#define __SPI_FLASH_REGISTER_H__

#define SPI_FLASH_WRITE_ENABLE                                  0x06
#define SPI_FLASH_WRITE_DISABLE                                 0x04

#define SPI_FLASH_READ_STATUS_REGISTER1                         0x05
#define SPI_FLASH_READ_STATUS_REGISTER2                         0x35
#define SPI_FLASH_READ_STATUS_REGISTER3                         0x15

#define SPI_FLASH_WRITE_STATUS_REGISTER1                        0x01
#define SPI_FLASH_WRITE_STATUS_REGISTER2                        0x31
#define SPI_FLASH_WRITE_STATUS_REGISTER3                        0x11

#define SPI_FLASH_READ_EXTENDED_ADDRESS_REGISTER                0xC8
#define SPI_FLASH_WRITE_EXTENDED_ADDRESS_REGISTER               0xC5

#define SPI_FLASH_ENTER_4BYTE_ADDRESS_MODE                      0xB7
#define SPI_FLASH_EXIT_4BYTE_ADDRESS_MODE                       0xE9

#define SPI_FLASH_READ_DATA                                     0x03
#define SPI_FLASH_READ_DATA_WITH_4BYTE_ADDRESS                  0x13
#define SPI_FLASH_FAST_READ                                     0x0B
#define SPI_FLASH_FAST_READ_WITH_4BYTE_ADDRESS                  0x0C
#define SPI_FLASH_FAST_READ_DUAL_OUTPUT                         0x3B
#define SPI_FLASH_FAST_READ_DUAL_OUTPUT_WITH_4BYTE_ADDRESS      0x3C
#define SPI_FLASH_FAST_READ_QUAD_OUTPUT                         0x6B
#define SPI_FLASH_FAST_READ_QUAD_OUTPUT_WITH_4BYTES_ADDRESS     0x6C
#define SPI_FLASH_FAST_READ_DUAL_IO                             0xBB
#define SPI_FLASH_FAST_READ_DUAL_IO_WITH_4BYTES_ADDRESS         0xBC
#define SPI_FLASH_FAST_READ_QUAD_IO                             0xEB
#define SPI_FLASH_FAST_READ_QUAD_IO_WITH_4BYTES_ADDRESS         0xEC

#define SPI_FLASH_WORD_READ_QUAD_IO                             0xE7
#define SPI_FLASH_OCTAL_WORD_READ_QUAD_IO                       0xE3

#define SPI_FLASH_SET_BURST_WITH_WARP                           0x77

#define SPI_FLASH_PAGE_PROGRAM                                  0x02
#define SPI_FLASH_QUAD_INPUT_PAGE_PROGRAM                       0x32

#define SPI_FLASH_SECTOR_ERASE_4KB                              0x20
#define SPI_FLASH_BLOCK_ERASE_32KB                              0x52
#define SPI_FLASH_BLOCK_ERASE_64KB                              0xD8
#define SPI_FLASH_CHIP_ERASE                                    0xC7

#define SPI_FLASH_ERASE_PROGRAM_SUSPEND                         0x75
#define SPI_FLASH_ERASE_PROGRAM_RESUME                          0x7A

#define SPI_FLASH_POWER_DOWN                                    0xB9

#define SPI_FLASH_RELEASE_POWER_DOWN_DEVICE_ID                  0xAB
#define SPI_FLASH_MANUFACTURER_DEVICE_ID                        0x90
#define SPI_FLASH_READ_UNIQUE_ID                                0x4B
#define SPI_FLASH_JEDEC_ID                                      0x9F

#define SPI_FLASH_HIGH_PERFORMANCE_MODE                         0xA3
#define SPI_FLASH_CONTINUOUS_READ_MODE_RESET                    0xFF

#define SPI_FLASH_DUMMY_BYTE                                    0xFF

#endif // !__SPI_FLASH_REGISTER_H__
