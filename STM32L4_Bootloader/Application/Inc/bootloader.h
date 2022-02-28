#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "typedef.h"
#include "stm32g0xx_hal_flash.h"


/******************************************************************************
 * Defines here
 *****************************************************************************/
/*#define FLASH_PAGE_SIZE                 0x00000800U    !< FLASH Page Size, 2 KBytes */
#define FLASH_USER_START_ADDR           ((UINT32)0x08008000)    /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR             ((UINT32)0x0801FFFF)    /* End @ of user Flash area */
#define FLASH_USER_START_PAGE           ((UINT32)8)             /* Start Page No of user  Flash area */
#define FLASH_USER_END_PAGE             ((UINT32)31)            /* End   Page No of user  Flash area */
#define FLASH_USER_SIZE                 0xC000                  /* Total user Flash area 48KBytes */
#define NO_OF_USER_PAGES                24                      /* 1 page = 2048 bytes
                                                                  So, C000 = 24 pages */
#define CARD_ID                         ePRINTER_CARD_CONNECTED

//#define FLASH_PAGES_TO_BE_PROTECTED     (OB_WRP_AllPages)

#define DEVICE_UNIQUE_ID_BASE_ADDRESS   UID_BASE
#define DEVICE_UNIQUE_ID_BYTES_LENGTH   12


#define FLASH_BOOTLOADER_START_ADDR     ((UINT32)0x08000000)    /* Start @ of user Bootloader Flash area */
#define FLASH_BOOTLOADER_END_ADDR       ((UINT32)0x08003FFF)    /* End @ of user Bootloader Flash area */
#define FLASH_BOOTLOADER_START_PAGE     ((UINT32)0)             /* Start Page No of user Bootloader Flash area */
#define FLASH_BOOTLOADER_END_PAGE       ((UINT32)7)             /* End   Page No of user Bootloader Flash area */
#define FLASH_SIZE_BOOTLOADER           0x4000                  /* Total Bootloader Flash area 16KBytes */
#define NO_OF_BOOTLOADER_PAGES          8
#define APPLICATION_ADDRESS             FLASH_USER_START_ADDR
#define COMM_INTERVAL_RATE              5
#define PROGRAMING_MODE_TIMEOUT_RATE    1000
/******************************************************************************
* Enums here
******************************************************************************/
typedef enum
{
    eCOMM_IDLE_STATE,
    eCOMM_RX_ON,
    eCOMM_TX_ON,
    eCOMM_RX_DONE
}TypedefCommTxRx;

typedef enum
{
    eBOOTLOADER_PROGRAM_FLASH,          //0
    eBOOTLOADER_IDLE                    //1
}TypedefBootloader;

typedef enum
{
    eNO_CARD = 0,
    eCPU_CARD_CONNECTED,
    eSENSOR_CARD_CONNECTED,
    eUDK_CARD_CONNECTED,
    eSKBD_CARD_CONNECTED,
    ePRINTER_CARD_CONNECTED,
    eIMPRINT_CARD_CONNECTED,
    eE_LOCK_CARD_CONNECTED,
    eCVRS_CARD_CONNECTED,
    eDEFAULT_CARD
}tdenCardID;
/******************************************************************************
* Structs here
******************************************************************************/


/******************************************************************************
* Public function prototypes (externed from)
******************************************************************************/


/******************************************************************************
* Private function prototypes
******************************************************************************/
extern void Bootloader_ModuleHandling(void);
extern void Bootloader_JumpToApplication(void);
extern void Bootloader_CalculateAndSendFlashChecksum(void);
extern void Bootloader_SendAck(void);
extern void Bootloader_SendNack(void);
extern void Bootloader_CheckProgrammingMode(void);
extern void Bootloader_CalculateAndSendUniqueIDBootloaderVersionNoChecksumAndSHA(void);
extern void Bootloader_GetProprietaryDynamicKey (void);
typedef  void (*pFunction)(void);
/******************************************************************************
* Public Variable here (externed from)
******************************************************************************/
extern UINT16 g_u16ServiceLEDScaler;        /* Scalar for LED Toggle */
extern UINT8 g_u8BootStage;
/******************************************************************************
* Private Variables here
******************************************************************************/

#endif /*_BOOTLOADER_H_*/
