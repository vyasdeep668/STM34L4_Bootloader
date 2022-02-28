#ifndef _MAIN_H_
#define _MAIN_H_

/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_def.h"
#include "stm32g071xx.h"
#include "typedef.h"
#include "FirmwareSignature_SHA256.h"
#include "gpio.h"
#include "Communication.h"


/******************************************************************************
 * Defines here
 *****************************************************************************/
#define SERVICE_LED_ON                      HAL_GPIO_WritePin(SERVICE_LED_PORT, SERVICE_LED_PIN, GPIO_PIN_RESET)
#define SERVICE_LED_OFF                     HAL_GPIO_WritePin(SERVICE_LED_PORT, SERVICE_LED_PIN, GPIO_PIN_SET)

#define COMM_TRANSMIT_MODE                  HAL_GPIO_WritePin(SERIAL_DIR_PORT,  SERIAL_DIR_PIN,  GPIO_PIN_SET)
#define COMM_RECEIVE_MODE                   HAL_GPIO_WritePin(SERIAL_DIR_PORT,  SERIAL_DIR_PIN,  GPIO_PIN_RESET)

#define NIBBLE_SHIFT                          4
#define BYTE_SHIFT                            8
#define WORD_SHIFT                            16

#define SERVICE_LED_TOGGLE_RATE               100
#define SERVICE_LED_TOGGLE_RATE_1             500
#define FLASH_LOCK_CMD                        0x31
#define FLASH_UNLOCK_CMD                      0x33
/******************************************************************************
* Enums here
******************************************************************************/
/* Enum for different system stages*/
typedef enum
{
    eSYSTEM_POWER_ON,
    eSYSTEM_CHECK_PROGRAMING_MODE,
    eSYSTEM_GET_PROPRIETARY_DYNAMIC_KEY,
    eSYSTEM_ENTER_PROGRAMING_MODE,
    eSYSTEM_CALCULATE_FIRMWARE_SHA_256_SIGNATURE_AND_VERIFY,
    eSYSTEM_CLEAR_FLASH_AREA,
    eSYSTEM_JUMP_TO_APPLICATION,
    eSYSTEM_DEFAULT_STAGE
}TypedefSystemStages;
/******************************************************************************
* Structs here
******************************************************************************/


/******************************************************************************
* Public function prototypes (externed from)
******************************************************************************/
extern UINT8 g_u8SystemStage;
extern UINT32 g_u32Address;
extern UINT32 g_u32PageNumber;
extern const UINT32 g_cu32BootloaderSoftwareVersion;
extern unsigned char g_u8arrFlash_SHA256_Signature_Received[RSA_ENCRYPTED_SHA_VALUE];
extern unsigned char g_u8TypeOfOperation;

/******************************************************************************
* Private function prototypes
******************************************************************************/
/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
/******************************************************************************
* Public Variable here (externed from)
******************************************************************************/

/******************************************************************************
* Private Variables here
******************************************************************************/

#endif /*_MAIN_H_*/
