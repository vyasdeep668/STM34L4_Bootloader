#ifndef COMMUNICATION_H
#define COMMUNICATION_H

//Include
#include "bootloader.h"

//Macros

#define PROTOCOL_START_BYTE     0x66
#define PROTOCOL_STOP_BYTE      0x99


//Command ID's

#define COMMAND_ID_BOOTLOADER_INFORMATION       0x10
#define COMMAND_ID_PROGRAMMING_MODE             0x11
#define COMMAND_ID_PROPRIETARY_DYNAMIC_KEY      0x12
#define COMMAND_ID_FLASH_HEX_DATA               0x13
#define COMMAND_ID_ERASE_FLASH                  0x14
#define COMMAND_ID_SHA_256_SIGNATURE            0x15
#define COMMAND_ID_APPLICATION_CODE_CHECKSUM    0x16
#define COMMAND_ID_ACK                          0x05
#define COMMAND_ID_NACK                         0x07

//Type of Operation
#define CARD_PROGRAMMING_MODE                   0x01
#define DISPENSER_PROGRAMMING_MODE              0x02
#define ID_SETTING_MODE                         0x03
#define DEFAULT_VALUE                           0xFF


//ACK and NACK values
#define PROTOCOL_PACKET_ACK_VALUE               0xE0
#define SHA_COMPARISON_ACK_VALUE                0xE1

#define PROTOCOL_PACKET_NACK_VALUE              0xD0
#define SHA_COMPARISON_NACK_VALUE               0xD1


//RSA 1024 Encrypted SHA 256 value bytes
#define RSA_ENCRYPTED_SHA_VALUE                 128

#define RX_MAX_BLOCK_LENGTH                     1050
#define TX_MAX_BLOCK_LENGTH                     100

//Enums

typedef enum
{
   eSTART_BYTE = 0,
   eCOMMAND_ID,
   eNUMBER_OF_DATA_BYTES_MSB,
   eNUMBER_OF_DATA_BYTES_LSB,
   eDATA_FIELD,
   STOP_BYTE
}tdenCommunicationProtocol;

//Structure Definitions

typedef struct
{
    UINT8 u8CommLimit;
    UINT8 aryu8TransmitBuffer[TX_MAX_BLOCK_LENGTH];
    UINT16 u16CommIndex;
    UINT16 u16CommIntervalScaler;
    UINT32 u32ProgrammingModeScaler;
    BOOL bExitProgrammingMode;
    TypedefCommTxRx enCommTxRxMode;
}tdstCommunicationStruct;

typedef struct
{
    UINT8 aryu8ReceiveBuffer[RX_MAX_BLOCK_LENGTH];
    UINT16 u16NoOfDataBytesRxd;
    UINT8 u8CommandID;
}tdstComm_DataReceived;


//Extern Variables
extern tdstCommunicationStruct gstCommunicationStruct;
extern tdstComm_DataReceived gstComm_DataReceived;

extern UINT8 g_aryu8ReceivedData[RX_MAX_BLOCK_LENGTH];

//Function Prototypes
extern void fn_CommunicationModuleHandling(void);
void fn_FormCommunicationPacketAndSend(unsigned char l_u8CommandID, unsigned char *l_u8ptrTransmitBuffer, unsigned short l_u16NumberOfBytes);
extern void ResetCommunicationVariables(void);
#endif //End of #ifndef COMMUNICATION_H