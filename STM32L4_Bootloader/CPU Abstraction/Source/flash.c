/******************************************************************************
 *
 *
 *    File name   : flash.c
 *    Description : handles flash related operations
 *    Layer       :
 *
 *    History :
 *
 *****************************************************************************/


/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "typedef.h"
#include "initialisation.h"
#include "gpio.h"
#include "main.h"

#include "bootloader.h"
#include "flash.h"
#include "Communication.h"
/******************************************************************************
 * Defines here
 *****************************************************************************/

/******************************************************************************
* Enums here
******************************************************************************/

/******************************************************************************
* Structs here
******************************************************************************/

/******************************************************************************
* Public function prototypes (externed from)
******************************************************************************/

/******************************************************************************
* Private function prototypes
******************************************************************************/

/******************************************************************************
* Public Variable here (externed from)
******************************************************************************/
extern UINT32 g_u32OptionbyteData;

/******************************************************************************
* Private Variables here
******************************************************************************/
HAL_StatusTypeDef FLASHStatus = HAL_OK;
FLASH_OBProgramInitTypeDef FLASH_OBProgramInitHandle;


/*************************************************************************************************************
 *
 * @Function		  	        unsigned char Flash_WriteOperations(void)
 *
 * @Created By 	  		        Deep Vyas
 *
 * @Brief		  	        Write word data into flash area
 *
 * @param[in]                           None
 * @param[out]                          None
 *
 * @Return		                Flash write status
 *
 * @Pass/Fail criteria                  None
 *************************************************************************************************************/
unsigned char Flash_WriteOperations(void)
{

    UINT64 l_u64ProgramDoubleWordData = 0;
    unsigned char l_u8ReturnValue = 0;
    unsigned short l_u16LoopCounter = 0;

    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();

    /* Clear pending flags (if any) */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR);

    for (l_u16LoopCounter = 0 ; l_u16LoopCounter < gstComm_DataReceived.u16NoOfDataBytesRxd; l_u16LoopCounter += 8)
    {
        /* Write the Flash with Recieved Data */
        l_u64ProgramDoubleWordData  = (UINT64) (gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD + l_u16LoopCounter + 7]) << (BYTE_SHIFT*7) ;
        l_u64ProgramDoubleWordData |= (UINT64) (gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD + l_u16LoopCounter + 6]) << (BYTE_SHIFT*6) ;
        l_u64ProgramDoubleWordData |= (UINT64) (gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD + l_u16LoopCounter + 5]) << (BYTE_SHIFT*5) ;
        l_u64ProgramDoubleWordData |= (UINT64) (gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD + l_u16LoopCounter + 4]) << (BYTE_SHIFT*4) ;
        l_u64ProgramDoubleWordData |= (UINT64) (gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD + l_u16LoopCounter + 3]) << (BYTE_SHIFT*3) ;
        l_u64ProgramDoubleWordData |= (UINT64) (gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD + l_u16LoopCounter + 2]) << (BYTE_SHIFT*2) ;
        l_u64ProgramDoubleWordData |= (UINT64) (gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD + l_u16LoopCounter + 1]) << (BYTE_SHIFT*1) ;
        l_u64ProgramDoubleWordData |= (UINT64) (gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD + l_u16LoopCounter + 0]) << (BYTE_SHIFT*0) ;

        if ( HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, g_u32Address, l_u64ProgramDoubleWordData ) == HAL_OK )
        {
            g_u32Address = g_u32Address + 8;
            l_u8ReturnValue = HAL_OK;
            l_u64ProgramDoubleWordData = 0;
        }
        else
        {
            l_u8ReturnValue = 0;
            l_u64ProgramDoubleWordData = 0;
            break;
        }
    }

    /* Lock the Flash */
    HAL_FLASH_Lock();

  return l_u8ReturnValue;
}

/*************************************************************************************************************
 *
 * @Function		  	        void Flash_FullFlashErase(void)
 *
 * @Created By 	  		        Deep Vyas
 *
 * @Brief		  	        Erase the User flash area
 *
 * @param[in]                           None
 * @param[out]                          None
 *
 * @Return		                None
 *
 * @Pass/Fail criteria                  None
 *************************************************************************************************************/
void Flash_FullFlashErase(void)
{
    FLASH_EraseInitTypeDef FLASH_EraseStruct;
    uint32_t l_u32PageError = HAL_FLASH_ERROR_NONE;

    /*Configure FLASH_Erase Structure*/
    FLASH_EraseStruct.TypeErase = FLASH_TYPEERASE_PAGES;        /*!< Pages erase only */
    FLASH_EraseStruct.Banks = FLASH_BANK_1;
    FLASH_EraseStruct.Page = FLASH_USER_START_PAGE;             /*!< Initial Flash page to erase when page erase is enabled */
    FLASH_EraseStruct.NbPages = NO_OF_USER_PAGES;               /*!< Number of pages to be erased. */

    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();

    /* Clear pending flags (if any) */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR);

    /* Erase the FLASH pages */
    HAL_FLASHEx_Erase(&FLASH_EraseStruct, &l_u32PageError);

    /* Lock the Flash */
    HAL_FLASH_Lock();
}

/*************************************************************************************************************
 *
 * @Function		  	        void Flash_PageFlashErase( UINT32 l_u32PageStartAddress )
 *
 * @Created By 	  		        Deep Vyas
 *
 * @Brief		  	        This function is used to erase a single flash page
 *
 * @param[in]                           UINT32 l_u32PageStartAddress - Start Address of flash page
 * @param[out]                          None
 *
 * @Return		                None
 *
 * @Pass/Fail criteria                  None
 *************************************************************************************************************/
void Flash_PageFlashErase( UINT32 l_u32PageStartAddress )
{
    FLASH_EraseInitTypeDef FLASH_EraseStruct;
    uint32_t l_u32PageError = HAL_FLASH_ERROR_NONE;
    uint32_t l_u32PageNumber = (FLASH_USER_START_PAGE + ( (l_u32PageStartAddress - FLASH_USER_START_ADDR) /FLASH_PAGE_SIZE) );


    /*Configure FLASH_Erase Structure*/
    FLASH_EraseStruct.TypeErase = FLASH_TYPEERASE_PAGES;        /*!< Pages erase only */
    FLASH_EraseStruct.Banks = FLASH_BANK_1;
    FLASH_EraseStruct.Page = l_u32PageNumber;                   /*!< Initial Flash page to erase when page erase is enabled */
    FLASH_EraseStruct.NbPages = 1;                              /*!< Number of pages to be erased. */

    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();

    /* Clear pending flags (if any) */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR);

    /* Erase the FLASH pages */
    HAL_FLASHEx_Erase(&FLASH_EraseStruct, &l_u32PageError);

    /* Lock the Flash */
    HAL_FLASH_Lock();
}

/*************************************************************************************************************
 *
 * @Function		  	        void Flash_MemoryLock(void)
 *
 * @Created By 	  		        Deep Vyas
 *
 * @Brief		  	        Lock the Whole flash
 *                                      This operation triggers a soft reset
 *
 * @param[in]                           None
 * @param[out]                          None
 *
 * @Return		                None
 *
 * @Pass/Fail criteria                  None
 *************************************************************************************************************/

void Flash_MemoryLock(void)
{

    HAL_FLASH_Unlock();

    HAL_FLASH_OB_Unlock();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR);

    /* Get pages write protection status */
    HAL_FLASHEx_OBGetConfig(&FLASH_OBProgramInitHandle);

/*    |---------------------------------------------------------------------|
      |                        WRP registers values                         |
      |---------------------------------------------------------------------|
      |          Condition             |          WRP-protected area        |
      |--------------------------------|------------------------------------|
      |  WRP1A_STRT = WRP1A_ENDPage    |               WRP1A                |
      |  WRP1A_STRT > WRP1A_ENDNone    |           (unprotected)            |
      |  WRP1A_STRT < WRP1A_ENDPages   |     from WRP1A_STRT to WRP1A_END   |
      |---------------------------------------------------------------------|*/

      /* Check if desired pages are not yet write protected */
    if(FLASH_OBProgramInitHandle.WRPStartOffset > FLASH_OBProgramInitHandle.WRPEndOffset)
    {
        /* Enable the pages write protection */
        FLASH_OBProgramInitHandle.OptionType = OPTIONBYTE_WRP;                     /*!< WRP option byte configuration*/
        FLASH_OBProgramInitHandle.WRPArea = OB_WRPAREA_ZONE_A;                     /*!< Flash Zone A */
        FLASH_OBProgramInitHandle.WRPStartOffset = FLASH_BOOTLOADER_START_PAGE;                   /* Page 00 - 0x0800 0000*/
        FLASH_OBProgramInitHandle.WRPEndOffset = (NO_OF_BOOTLOADER_PAGES + NO_OF_USER_PAGES) - 1; /* Page 31 - 0x0800 FFFF*/
        FLASHStatus = HAL_FLASHEx_OBProgram(&FLASH_OBProgramInitHandle);

        /* Generate System Reset to load the new option byte values */
        HAL_FLASH_OB_Launch();

        HAL_FLASH_OB_Lock();
    }
    else
    {}
}

/*************************************************************************************************************
 *
 * @Function		  	    void Flash_MemoryUnlock(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	    Unlock the Whole flash
 *                                  This operation triggers a soft reset.
 *
 * @param[in]                       None
 * @param[out]                      None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria              None
 *************************************************************************************************************/
void Flash_MemoryUnlock(void)
{
    HAL_FLASH_Unlock();

    HAL_FLASH_OB_Unlock();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR);

    /* Get pages write protection status */
    HAL_FLASHEx_OBGetConfig(&FLASH_OBProgramInitHandle);

/*    |---------------------------------------------------------------------|
      |                        WRP registers values                         |
      |---------------------------------------------------------------------|
      |          Condition             |          WRP-protected area        |
      |--------------------------------|------------------------------------|
      |  WRP1A_STRT = WRP1A_ENDPage    |               WRP1A                |
      |  WRP1A_STRT > WRP1A_ENDNone    |           (unprotected)            |
      |  WRP1A_STRT < WRP1A_ENDPages   |     from WRP1A_STRT to WRP1A_END   |
      |---------------------------------------------------------------------|*/

      /* Check if desired pages are write protected */
    if(FLASH_OBProgramInitHandle.WRPStartOffset <= FLASH_OBProgramInitHandle.WRPEndOffset)
    {
        /* Enable the pages write protection */
        FLASH_OBProgramInitHandle.OptionType = OPTIONBYTE_WRP;                     /*!< WRP option byte configuration*/
        FLASH_OBProgramInitHandle.WRPArea = OB_WRPAREA_ZONE_A;                     /*!< Flash Zone A */
        FLASH_OBProgramInitHandle.WRPStartOffset = (UINT32)0xFF;
        FLASH_OBProgramInitHandle.WRPEndOffset = (UINT32)0x00;
        FLASHStatus = HAL_FLASHEx_OBProgram(&FLASH_OBProgramInitHandle);

        /* Generate System Reset to load the new option byte values */
        HAL_FLASH_OB_Launch();

        HAL_FLASH_OB_Lock();
    }
    else
    {}
}


/*************************************************************************************************************
 *
 * @Function		  	    void Flash_DisableSTBootloader(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		            Disables the st's default bootlaoder which is invoked by the boot1 pin of the
 *                                  controller (Boot entry is forced to Flash or System Flash)
 *
 * @param[in]                       None
 * @param[out]                      None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria              None
 *************************************************************************************************************/
void Flash_DisableSTBootloader( void )
{
    HAL_FLASH_Unlock();

    HAL_FLASH_OB_Unlock();

    /* Get pages write protection status */
    HAL_FLASHEx_OBGetConfig(&FLASH_OBProgramInitHandle);

    if(FLASH_OBProgramInitHandle.BootEntryPoint == OB_BOOT_ENTRY_FORCED_NONE)
    {
        FLASH_OBProgramInitHandle.OptionType = OPTIONBYTE_SEC;      /*!< SEC option byte configuration */
        FLASH_OBProgramInitHandle.BootEntryPoint = OB_BOOT_ENTRY_FORCED_FLASH;
        FLASH_OBProgramInitHandle.SecSize = NO_OF_BOOTLOADER_PAGES + NO_OF_USER_PAGES;
        FLASHStatus = HAL_FLASHEx_OBProgram(&FLASH_OBProgramInitHandle);

        /* Generate System Reset to load the new option byte values */
        HAL_FLASH_OB_Launch();

        HAL_FLASH_OB_Lock();
    }
    else
    {}
}
/*************************************************************************************************************
 *
 * @Function		  	    void Flash_EnableSTBootloader(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		            Enables the st's default bootlaoder which is invoked by the boot1 pin of the
 *                                  controller (Boot entry is forced to Flash or System Flash)
 *
 * @param[in]                       None
 * @param[out]                      None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria              None
 *************************************************************************************************************/
void Flash_EnableSTBootloader(void)
{
    HAL_FLASH_Unlock();

    HAL_FLASH_OB_Unlock();

    /* Get pages write protection status */
    HAL_FLASHEx_OBGetConfig(&FLASH_OBProgramInitHandle);

    if(FLASH_OBProgramInitHandle.BootEntryPoint == OB_BOOT_ENTRY_FORCED_FLASH)
    {
        /*Forcing boot from Flash memory. It is possible to reset it only when: RDP is set to Level 0*/
        Flash_Level1ProtectionDisable();
        FLASH_OBProgramInitHandle.OptionType = OPTIONBYTE_SEC;      /*!< SEC option byte configuration */
        FLASH_OBProgramInitHandle.BootEntryPoint = OB_BOOT_ENTRY_FORCED_NONE;
        FLASH_OBProgramInitHandle.SecSize = 0;
        FLASHStatus = HAL_FLASHEx_OBProgram(&FLASH_OBProgramInitHandle);

        /* Generate System Reset to load the new option byte values */
        HAL_FLASH_OB_Launch();

        HAL_FLASH_OB_Lock();
    }
    else
    {}
}

/*************************************************************************************************************
 *
 * @Function		  	    void Flash_Level1ProtectionEnable(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		            Disable External Debugger Intrusion
 *
 * @param[in]                       None
 * @param[out]                      None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria              None
 *************************************************************************************************************/
void Flash_Level1ProtectionEnable(void)
{
    HAL_FLASH_Unlock();

    HAL_FLASH_OB_Unlock();

    /* Get Option Byte status */
    HAL_FLASHEx_OBGetConfig(&FLASH_OBProgramInitHandle);

    if(FLASH_OBProgramInitHandle.RDPLevel == OB_RDP_LEVEL_0)
    {
        /* Enable RDP Level 1 Protection */
        FLASH_OBProgramInitHandle.OptionType = OPTIONBYTE_RDP;                     /*!< RDP option byte configuration*/
        FLASH_OBProgramInitHandle.RDPLevel = OB_RDP_LEVEL_1;
        FLASHStatus = HAL_FLASHEx_OBProgram(&FLASH_OBProgramInitHandle);

        /* Generate System Reset to load the new option byte values */
        HAL_FLASH_OB_Launch();

        HAL_FLASH_OB_Lock();
    }
    else
    {}
}

/*************************************************************************************************************
 *
 * @Function		  	    void Flash_Level1ProtectionDisable(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		            Enable External Debugger Intrusion
 *
 * @param[in]                       None
 * @param[out]                      None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria              None
 *************************************************************************************************************/
void Flash_Level1ProtectionDisable(void)
{
    HAL_FLASH_Unlock();

    HAL_FLASH_OB_Unlock();

    /* Get Option Byte status */
    HAL_FLASHEx_OBGetConfig(&FLASH_OBProgramInitHandle);

    if(FLASH_OBProgramInitHandle.RDPLevel == OB_RDP_LEVEL_1)
    {
        /*RDP Level 0 Protection */
        FLASH_OBProgramInitHandle.OptionType = OPTIONBYTE_RDP;                     /*!< RDP option byte configuration*/
        FLASH_OBProgramInitHandle.RDPLevel = OB_RDP_LEVEL_0;
        FLASHStatus = HAL_FLASHEx_OBProgram(&FLASH_OBProgramInitHandle);

        /* Generate System Reset to load the new option byte values */
        HAL_FLASH_OB_Launch();

        HAL_FLASH_OB_Lock();
    }
    else
    {}
}
