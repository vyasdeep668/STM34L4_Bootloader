/******************************************************************************
 *
 *
 *    File name   : bootloader.c
 *    Description : Bootloader on RS 485 port
 *    Layer       : Application
 *
 *    History :
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
#include "FirmwareSignature_SHA256.h"
#include "Bootloader_Proprietary_Encryption.h"
#include "Communication.h"
#include <string.h>
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
extern BOOL bClaibSWStatus;

/******************************************************************************
* Private Variables here
******************************************************************************/
UINT8 g_u8BootStage;

/*************************************************************************************************************
 *
 * @Function		  	    void Bootloader_ModuleHandling(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        Handles the Bootloader operation for flash wrie
 *
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void Bootloader_ModuleHandling(void)
{
    switch ( g_u8BootStage )
    {
        case eBOOTLOADER_PROGRAM_FLASH:
        {
            if( HAL_OK == Flash_WriteOperations() )
            {
                Bootloader_SendAck();

                /*if all flash bytes are programmed set stage to idle*/
                if( g_u32Address == FLASH_USER_END_ADDR + 1 )
                {
                    g_u8BootStage = eBOOTLOADER_IDLE;
                }
            }
            else
            {
                g_u32Address -= (g_u32Address % FLASH_PAGE_SIZE);
                Flash_PageFlashErase(g_u32Address);
            }

            g_u8SystemStage = eSYSTEM_DEFAULT_STAGE;
        }
        break;

        case eBOOTLOADER_IDLE:
        default:
        {
        }
        break;
    }
}

/*************************************************************************************************************
 *
 * @Function		  	    void Bootloader_JumpToApplication(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        Jumps from Bootloader application to User(Evole) Application
 *
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void Bootloader_JumpToApplication(void)
{
    UINT32 l_u32JumpAddress;
    pFunction Jump_To_Application;

    /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
    if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    {
        /* Jump to user application */
        l_u32JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
        Jump_To_Application = (pFunction) l_u32JumpAddress;
        
        HAL_RCC_DeInit();
        HAL_DeInit();
        
        //SysTick->CTRL = 0;
        //SysTick->LOAD = 0;
        //SysTick->VAL = 0;
        
        //SCB->VTOR = APPLICATION_ADDRESS;

        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);

        /* Jump to application */
        Jump_To_Application();
    }
}

/*************************************************************************************************************
 *
 * @Function		  	    void Bootloader_CalculateAndSendFlashChecksum(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This function is used calculate and send the flash checksum.
 *
 * @param[in]               None
 *
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void Bootloader_CalculateAndSendFlashChecksum(void)
{
    UINT32 u32AddressOffset = 0;
    UINT32 u32FlashBaseAddress = 0;
    UINT32 u32FlashAddress = 0;
    UINT32 l_u32FlashChecksum = 0;

    u32FlashBaseAddress = (UINT32)(FLASH_USER_START_ADDR);

    for ( u32AddressOffset = 0; u32AddressOffset < (UINT32)FLASH_USER_SIZE; u32AddressOffset+= 4)
    {
        u32FlashAddress = u32FlashBaseAddress + u32AddressOffset;
        l_u32FlashChecksum += *((UINT32 *)u32FlashAddress);
    }

    gstCommunicationStruct.aryu8TransmitBuffer[0] = (UINT8)( l_u32FlashChecksum >> ( WORD_SHIFT + BYTE_SHIFT ) );
    gstCommunicationStruct.aryu8TransmitBuffer[1] = (UINT8)( l_u32FlashChecksum >> WORD_SHIFT );
    gstCommunicationStruct.aryu8TransmitBuffer[2] = (UINT8)( l_u32FlashChecksum >> BYTE_SHIFT );
    gstCommunicationStruct.aryu8TransmitBuffer[3] = (UINT8)( l_u32FlashChecksum );

    fn_FormCommunicationPacketAndSend(COMMAND_ID_APPLICATION_CODE_CHECKSUM, gstCommunicationStruct.aryu8TransmitBuffer, 4);
}

/*************************************************************************************************************
 *
 * @Function		  	    void Bootloader_SendAck(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This functions sends the ACK value to programming tool to indicate
                            that the previous packet was received successfully
 *
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void Bootloader_SendAck(void)
{
    if( g_u8SystemStage != eSYSTEM_CALCULATE_FIRMWARE_SHA_256_SIGNATURE_AND_VERIFY )
    {
        gstCommunicationStruct.aryu8TransmitBuffer[0] = PROTOCOL_PACKET_ACK_VALUE;
        fn_FormCommunicationPacketAndSend(COMMAND_ID_ACK, gstCommunicationStruct.aryu8TransmitBuffer, 1);
    }
    else
    {
        gstCommunicationStruct.aryu8TransmitBuffer[0] = SHA_COMPARISON_ACK_VALUE;
        fn_FormCommunicationPacketAndSend(COMMAND_ID_ACK, gstCommunicationStruct.aryu8TransmitBuffer, 1);
    }
}

/*************************************************************************************************************
 *
 * @Function		  	    void Bootloader_SendNack(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	         This functions sends the NACK value to programming tool to indicate
                            that the previous packet was not received successfully
 *
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void Bootloader_SendNack(void)
{
    if( g_u8SystemStage != eSYSTEM_CALCULATE_FIRMWARE_SHA_256_SIGNATURE_AND_VERIFY )
    {
        gstCommunicationStruct.aryu8TransmitBuffer[0] = PROTOCOL_PACKET_NACK_VALUE;
        fn_FormCommunicationPacketAndSend(COMMAND_ID_NACK, gstCommunicationStruct.aryu8TransmitBuffer, 1);
    }
    else
    {
        gstCommunicationStruct.aryu8TransmitBuffer[0] = SHA_COMPARISON_NACK_VALUE;
        fn_FormCommunicationPacketAndSend(COMMAND_ID_NACK, gstCommunicationStruct.aryu8TransmitBuffer, 1);
    }

}

/*************************************************************************************************************
 *
 * @Function		  	    void Bootloader_CalculateAndSendUniqueIDBootloaderVersionNoChecksumAndSHA(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This function calculates the Bootloader checksum and SHA and also sends the same
 *                          along with Bootloader Version Number and Controller Unique ID to Programming Tool.
 *
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void Bootloader_CalculateAndSendUniqueIDBootloaderVersionNoChecksumAndSHA(void)
{
    unsigned char l_u8LoopCounter = 0;
    UINT32 u32AddressOffset = 0;
    UINT32 u32FlashBaseAddress = 0;
    UINT32 u32FlashAddress = 0;
    UINT32 l_u32BootlaoderChecksum = 0;
    UINT8 *pu8UniqueIdBasePtr = (UINT8 *)DEVICE_UNIQUE_ID_BASE_ADDRESS;

    u32FlashBaseAddress = (UINT32)(FLASH_BOOTLOADER_START_ADDR);

    for ( u32AddressOffset = 0; u32AddressOffset < (UINT32)FLASH_SIZE_BOOTLOADER; u32AddressOffset+= 4)
    {
      u32FlashAddress = u32FlashBaseAddress + u32AddressOffset;
      l_u32BootlaoderChecksum += *((UINT32 *)u32FlashAddress);
    }

    CalculateFirmwareSignature(FLASH_BOOTLOADER_START_ADDR, FLASH_BOOTLOADER_END_ADDR);

    gstCommunicationStruct.aryu8TransmitBuffer[0] =  CARD_ID; //Card ID

    // Read the Device Unique ID
    memcpy(&gstCommunicationStruct.aryu8TransmitBuffer[1],pu8UniqueIdBasePtr,DEVICE_UNIQUE_ID_BYTES_LENGTH);

    /* Send the calculated Checksum SHA 256 and Version Number to Programming Tool */
    gstCommunicationStruct.aryu8TransmitBuffer[13] = (UINT8)( g_cu32BootloaderSoftwareVersion >> ( BYTE_SHIFT + WORD_SHIFT ) );
    gstCommunicationStruct.aryu8TransmitBuffer[14] = (UINT8)( g_cu32BootloaderSoftwareVersion >> WORD_SHIFT );
    gstCommunicationStruct.aryu8TransmitBuffer[15] = (UINT8)( g_cu32BootloaderSoftwareVersion >> BYTE_SHIFT );
    gstCommunicationStruct.aryu8TransmitBuffer[16] = (UINT8)( g_cu32BootloaderSoftwareVersion );
    gstCommunicationStruct.aryu8TransmitBuffer[17] = (UINT8)( l_u32BootlaoderChecksum >> ( WORD_SHIFT + BYTE_SHIFT ) );
    gstCommunicationStruct.aryu8TransmitBuffer[18] = (UINT8)( l_u32BootlaoderChecksum >> WORD_SHIFT );
    gstCommunicationStruct.aryu8TransmitBuffer[19] = (UINT8)( l_u32BootlaoderChecksum >> BYTE_SHIFT );
    gstCommunicationStruct.aryu8TransmitBuffer[20] = (UINT8)( l_u32BootlaoderChecksum );

    for(l_u8LoopCounter = 0;l_u8LoopCounter < SHA256_SIGNATURE_SIZE_IN_BYTES; l_u8LoopCounter++ )
    {
       gstCommunicationStruct.aryu8TransmitBuffer[(21 + l_u8LoopCounter)] = SHA256_Output_Struct.m_aryu8SHAOutput[l_u8LoopCounter];
    }

    fn_FormCommunicationPacketAndSend(COMMAND_ID_BOOTLOADER_INFORMATION, gstCommunicationStruct.aryu8TransmitBuffer, 53);
}
/*************************************************************************************************************
 *
 * @Function		  	    void Bootloader_CheckProgrammingMode(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This function checks the programming timeout value and whether the Programmin utility has
 *                          has responded with the operation type.
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void Bootloader_CheckProgrammingMode(void)
{
    if( RESET == gstCommunicationStruct.bExitProgrammingMode )
    {
        if ( (CARD_PROGRAMMING_MODE == g_u8TypeOfOperation) || (DISPENSER_PROGRAMMING_MODE == g_u8TypeOfOperation) )
        {
            gstCommunicationStruct.enCommTxRxMode = eCOMM_IDLE_STATE;
            gstCommunicationStruct.u16CommIntervalScaler = 0;
            gstCommunicationStruct.u32ProgrammingModeScaler = 0;
            SERVICE_LED_OFF;
            Flash_FullFlashErase();
            Bootloader_SendAck();

            g_u8SystemStage = eSYSTEM_DEFAULT_STAGE;

            g_u8TypeOfOperation = DEFAULT_VALUE;
        }
        else if( ID_SETTING_MODE == g_u8TypeOfOperation )
        {
            Bootloader_SendAck();
            g_u8TypeOfOperation = DEFAULT_VALUE;
        }
        else
        {
        }
     }
     else
     {
          g_u8SystemStage = eSYSTEM_JUMP_TO_APPLICATION;
     }
}
/*************************************************************************************************************
 *
 * @Function		  	    void Bootloader_GetProprietaryDynamicKey(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This function is used for receiving Dynamic key which will be used for
 *                          decrypting messages from Programming Tool
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void Bootloader_GetProprietaryDynamicKey (void)
{
    if( gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD] < 16 )
    {
        Bootloader_SendAck();
        fn_ProprietaryEncryptionAlgorithm_SetProprietaryDynamicKey( (UINT8) gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD] );
        g_u8BootStage = eBOOTLOADER_PROGRAM_FLASH;
    }
    else
    {
        Bootloader_SendNack();
    }
}