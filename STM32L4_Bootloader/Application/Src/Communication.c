/******************************************************************************
 *
 *
 *    File name   : Communication.c
 *    Description : This file includes coomunication related functions
 *    Layer       : Application
 *
 *    History :
 *
 *****************************************************************************/


/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "typedef.h"
#include "main.h"
#include "bootloader.h"
#include "FirmwareSignature_SHA256.h"
#include "Bootloader_Proprietary_Encryption.h"
#include "Communication.h"
#include "usart.h"
#include "Initialisation.h"
#include "string.h"

/******************************************************************************
 * Defines here
 *****************************************************************************/
UINT8 g_aryu8ReceivedData[RX_MAX_BLOCK_LENGTH];

/******************************************************************************
* Enums here
******************************************************************************/

/******************************************************************************
* Structs here
******************************************************************************/
tdstCommunicationStruct gstCommunicationStruct;
tdstComm_DataReceived gstComm_DataReceived;

/******************************************************************************
* Public function prototypes (externed from)
******************************************************************************/

/******************************************************************************
* Private function prototypes
******************************************************************************/

/******************************************************************************
* Public Variable here (externed from)
******************************************************************************/
extern UINT16 g_u16ServiceLEDScalerTemp;
/******************************************************************************
* Private Variables here
******************************************************************************/

/** @brief Reset Communication Variables
 *
 *  This function is used to clear the communication related variables
 *
 *  @param
 *  @return None
 */
void ResetCommunicationVariables(void)
{
    unsigned short l_u16LoopCounter = 0;

    gstCommunicationStruct.u16CommIntervalScaler = 0;
    gstCommunicationStruct.u16CommIndex = 0;
    gstCommunicationStruct.u8CommLimit = 0;

    gstCommunicationStruct.enCommTxRxMode = eCOMM_RX_ON;

    COMM_RECEIVE_MODE;

    //Clear Transmit And Receive Buffer
    for(l_u16LoopCounter = 0; l_u16LoopCounter < RX_MAX_BLOCK_LENGTH ; l_u16LoopCounter++)
    {
        gstComm_DataReceived.aryu8ReceiveBuffer[l_u16LoopCounter] = 0;
        g_aryu8ReceivedData[l_u16LoopCounter] = 0;
    }

    for(l_u16LoopCounter = 0; l_u16LoopCounter < TX_MAX_BLOCK_LENGTH ; l_u16LoopCounter++)
    {
        gstCommunicationStruct.aryu8TransmitBuffer[l_u16LoopCounter] = 0;
    }

    //Reset Receive Serial Variables
    gstComm_DataReceived.u8CommandID = 0x00;
    gstComm_DataReceived.u16NoOfDataBytesRxd = 0x0000;
}

/** @brief Communication Module Handling Function
 *
 *  This function is used to parse the serial bytes received
 *  on serial port
 *
 *  @param
 *  @return None
 */
void fn_CommunicationModuleHandling(void)
{
    unsigned short l_usLoopCounter = 0;

    if( gstCommunicationStruct.enCommTxRxMode == eCOMM_RX_DONE )
    {
        fn_ProprietaryEncryptionAlgorithm_DecryptData(g_aryu8ReceivedData, gstComm_DataReceived.aryu8ReceiveBuffer, gstCommunicationStruct.u16CommIndex);

        if( (gstComm_DataReceived.aryu8ReceiveBuffer[0] == PROTOCOL_START_BYTE) && (gstComm_DataReceived.aryu8ReceiveBuffer[gstCommunicationStruct.u16CommIndex - 1] == PROTOCOL_STOP_BYTE) )
        {
            gstComm_DataReceived.u8CommandID = gstComm_DataReceived.aryu8ReceiveBuffer[eCOMMAND_ID];
            gstComm_DataReceived.u16NoOfDataBytesRxd = ((gstComm_DataReceived.aryu8ReceiveBuffer[eNUMBER_OF_DATA_BYTES_MSB] << 8) & 0xFF00) ;
            gstComm_DataReceived.u16NoOfDataBytesRxd |= (gstComm_DataReceived.aryu8ReceiveBuffer[eNUMBER_OF_DATA_BYTES_LSB] & 0xFF);

            switch(gstComm_DataReceived.u8CommandID)
            {
                case COMMAND_ID_PROGRAMMING_MODE:
                {
                    g_u8TypeOfOperation = (UINT8)( gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD]);
                }
                break;

                case COMMAND_ID_PROPRIETARY_DYNAMIC_KEY:
                {
                    g_u8SystemStage = eSYSTEM_GET_PROPRIETARY_DYNAMIC_KEY;
                }
                break;

                case COMMAND_ID_FLASH_HEX_DATA:
                {
                    g_u8SystemStage = eSYSTEM_ENTER_PROGRAMING_MODE;
                }
                break;

                case COMMAND_ID_SHA_256_SIGNATURE:
                {
                    for(l_usLoopCounter = 0; l_usLoopCounter < RSA_ENCRYPTED_SHA_VALUE; l_usLoopCounter++)
                    {
                        g_u8arrFlash_SHA256_Signature_Received[l_usLoopCounter] = gstComm_DataReceived.aryu8ReceiveBuffer[eDATA_FIELD + l_usLoopCounter];
                    }

                    Bootloader_SendAck();
                    g_u8SystemStage = eSYSTEM_CALCULATE_FIRMWARE_SHA_256_SIGNATURE_AND_VERIFY;
                }
                break;

                default:
                {}
                break;
            }
        }
        else
        {
            ResetCommunicationVariables();
        }
    }
}

/** @brief fn_FormCommunicationPacketAndSend
 *
 *  This function is used to form communication packet and send
    the same on UART line to programming Tool
 *
 *  @param unsigned char l_u8CommandID - This parameter indicates the command ID
           unsigned char * l_u8ptrDataField - This parameter is an unsigned character pointer to data array
           unsigned short l_u16NumberOfBytes - This parameter indicates the number of bytes to be transmitted
 *  @return None
 */
void fn_FormCommunicationPacketAndSend(unsigned char l_u8CommandID, unsigned char *l_u8ptrTransmitBuffer, unsigned short l_u16NumberOfBytes)
{
    unsigned char l_u8LoopCounter = 0;
    unsigned char l_aryu8TransmitData[TX_MAX_BLOCK_LENGTH];

    l_aryu8TransmitData[eSTART_BYTE] = PROTOCOL_START_BYTE;
    l_aryu8TransmitData[eCOMMAND_ID] = l_u8CommandID;
    l_aryu8TransmitData[eNUMBER_OF_DATA_BYTES_MSB] = (unsigned char) ((l_u16NumberOfBytes & 0xFF00) >> 8);
    l_aryu8TransmitData[eNUMBER_OF_DATA_BYTES_LSB] = (unsigned char) (l_u16NumberOfBytes & 0x00FF);

    for(l_u8LoopCounter = 0; l_u8LoopCounter < l_u16NumberOfBytes; l_u8LoopCounter++)
    {
        l_aryu8TransmitData[eDATA_FIELD + l_u8LoopCounter] = l_u8ptrTransmitBuffer[l_u8LoopCounter];
    }

    l_aryu8TransmitData[STOP_BYTE + l_u16NumberOfBytes - 1] = PROTOCOL_STOP_BYTE;

    /* Number of bytes + 5(Start Byte(1), Command ID(1), NumberOfBytes(2), Stop Byte(1)) */
    gstCommunicationStruct.u8CommLimit = l_u16NumberOfBytes + 5;
    gstCommunicationStruct.u16CommIndex = 0;

    fn_ProprietaryEncryptionAlgorithm_EncryptData( l_aryu8TransmitData, l_u8ptrTransmitBuffer, gstCommunicationStruct.u8CommLimit );

    gstCommunicationStruct.enCommTxRxMode = eCOMM_TX_ON;
    COMM_TRANSMIT_MODE;

    LL_USART_EnableIT_TXE_TXFNF(SERIAL_UART);
}
