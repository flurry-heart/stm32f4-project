/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
// #define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
// #define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
// #define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_SD      0

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
    BYTE pdrv           /* Physical drive nmuber to identify the drive */
)
{
    int result;

    switch (pdrv) 
    {
    case DEV_SD :
        result = SD_Init();
        if (result)
        {
            return STA_NOINIT;
        }
    }
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    LBA_t sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to read */
)
{
    int result;

    switch (pdrv) 
    {
    case DEV_SD :
        result = SD_ReadData(&g_sd_handler, sector, count, buff);
        if (result)
        {
            return RES_PARERR;
        }
    }

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    LBA_t sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to write */
)
{
    int result;

    switch (pdrv) 
    {
    case DEV_SD :
        result = SD_WriteData(&g_sd_handler, sector, count, (uint8_t *)buff);
        if (result)
        {
            return RES_PARERR;
        }
    }

    return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT result = RES_ERROR;

    switch (pdrv) 
    {
    case DEV_SD :
        switch (cmd)
        {
        case CTRL_SYNC:
            result = RES_OK;
            break;
        case GET_SECTOR_SIZE:
            *(DWORD*)buff = 512;
            result = RES_OK;
            break;;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = g_sd_handler.SdCard.BlockSize;
            result = RES_OK;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = g_sd_handler.SdCard.BlockNbr * g_sd_handler.SdCard.BlockSize / 512;
            result = RES_OK;
            break;
  
        default:
            result = RES_PARERR;
        }
    }

    return result;
}