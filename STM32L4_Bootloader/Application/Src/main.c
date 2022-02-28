/******************************************************************************
 *
 *
 *    File name   : main.c
 *    Description : main code for Bootloader
 *    Layer       : Application
 *
 *    History :
 *
 *****************************************************************************/
/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "initialisation.h"
#include "main.h"
/******************************************************************************/
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
void SystemDelay(UINT32 DelayCount);
/******************************************************************************
* Public Variable here (externed from)
******************************************************************************/

/******************************************************************************
* Private Variables here
******************************************************************************/
UINT32 g_u32OptionbyteData;
UINT8 g_u8SystemStage;
UINT32 g_u32Address = 0;
UINT32 g_u32PageNumber = 0;
unsigned char g_u8arrFlash_SHA256_Signature_Received[RSA_ENCRYPTED_SHA_VALUE];
unsigned char g_u8TypeOfOperation = 0;
const UINT32 g_cu32BootloaderSoftwareVersion = 90000100;
/*************************************************************************************************************
 *
 * @Function		  	        void main( void )
 *
 * @Created By 	  		        Deep Vyas
 *
 * @Brief		  	            The Entry Point of code
 *
 * @param[in]                   None
 * @param[out]                  None
 *
 * @Return		                None
 *
 * @ Pass/ Fail criteria        None
 *************************************************************************************************************/
void main()
{
  /* STM32G0xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Low Level Initialization
  */
  /* MCU Configuration--------------------------------------------------------*/

  /* Delay at startup */
  SystemDelay(10000);

  /* Initialisation of different controller peripherals, clock and variables */
  DeviceInitialise();

  /* Copy User Flash Address to a global variable */
  g_u32Address = FLASH_USER_START_ADDR;

  /* Programing Mode Scaler is a timeout after this timeout the Bootloader will JumptoApplication */
  gstCommunicationStruct.u32ProgrammingModeScaler = PROGRAMING_MODE_TIMEOUT_RATE;

  /* CommIntervalScaler is a InterBlock (1KB) Timeout . CommIntervalScaler is 0 so that the timeout should not occure before the start of file transfer */
  gstCommunicationStruct.u16CommIntervalScaler = 0;

  /* Programing mode Timeout Flag */
  gstCommunicationStruct.bExitProgrammingMode = RESET;

  /* Set the Startup Stages of System and Bootloader */
  g_u8SystemStage = eSYSTEM_POWER_ON;
  g_u8BootStage = eBOOTLOADER_IDLE;

  /*Set Static Proprietary Key for initial communication*/
  fn_ProprietaryEncryptionAlgorithm_SetProprietaryDynamicKey(DEFAULT_PROPRIETARY_KEY);

    while (1)
    {
        fn_CommunicationModuleHandling();
        GPIO_ServiceLedHandler();

        switch(g_u8SystemStage)
        {
            case eSYSTEM_POWER_ON:
            {
                /* Send Bootloader Information on Serial Port*/
                Bootloader_CalculateAndSendUniqueIDBootloaderVersionNoChecksumAndSHA();

                g_u8SystemStage = eSYSTEM_CHECK_PROGRAMING_MODE;
            }
            break;

            case eSYSTEM_CHECK_PROGRAMING_MODE:
            {
                Bootloader_CheckProgrammingMode();
            }
            break;

            case eSYSTEM_GET_PROPRIETARY_DYNAMIC_KEY:
            {
                /* Get Proprietary Dynamic Key from Programming Tool */
                Bootloader_GetProprietaryDynamicKey();

                g_u8SystemStage = eSYSTEM_DEFAULT_STAGE;
            }
            break;

            case eSYSTEM_ENTER_PROGRAMING_MODE:
            {
                /* Program the flash memory based on the data received on serial port */
                Bootloader_ModuleHandling();
            }
            break;

            case eSYSTEM_CALCULATE_FIRMWARE_SHA_256_SIGNATURE_AND_VERIFY:
            {
                /* Calculate SHA 256 value for received Flash data */
                CalculateFirmwareSignature(FLASH_USER_START_ADDR, FLASH_USER_END_ADDR);

                /* Verify received RSA encrypted SHA value with Caluculated SHA value */
                if( 0x00000000 == RSA_1024_Verify(g_u8arrFlash_SHA256_Signature_Received, SHA256_Output_Struct.m_aryu8SHAOutput ) )              
                {
                    Bootloader_SendAck();

                    /* Delay to avoid mixing of 2 serial packets on Programming Utility Side */
                    while( eCOMM_TX_ON == gstCommunicationStruct.enCommTxRxMode );
                    SystemDelay(0xFFFFFF);

                    Bootloader_CalculateAndSendFlashChecksum();
                    g_u8SystemStage = eSYSTEM_DEFAULT_STAGE;
                    while( eCOMM_TX_ON == gstCommunicationStruct.enCommTxRxMode );
                }
                else
                {
                    Bootloader_SendNack();
                    Flash_FullFlashErase();
                    g_u8SystemStage = eSYSTEM_DEFAULT_STAGE;
                    while( eCOMM_TX_ON == gstCommunicationStruct.enCommTxRxMode );
                }
            }
            break;

            case eSYSTEM_CLEAR_FLASH_AREA:
            {
                Flash_FullFlashErase();
                Bootloader_SendAck();
                g_u8SystemStage = eSYSTEM_DEFAULT_STAGE;
                while( eCOMM_TX_ON == gstCommunicationStruct.enCommTxRxMode );
            }
            break;

            case eSYSTEM_JUMP_TO_APPLICATION:
            {
                SERVICE_LED_OFF;
                NVIC_DisableInterrupt();
                //HAL_TIM_Base_DeInit(&TIMER_Handle);
                //LL_USART_DeInit(SERIAL_UART);
                
                Bootloader_JumpToApplication();
            }
            break;

            case eSYSTEM_DEFAULT_STAGE:
            default:
            {
            }
            break;
        }
    }
}

/*************************************************************************************************************
 *
 * @Function		  	        void SystemDelay( UINT32 DelayCount)
 *
 * @Created By 	  		        Deep Vyas
 *
 * @Brief		  	            This function is used to generate a hard coded delay.
 *
 * @param[in]                   None
 * @param[out]                  None
 *
 * @Return		                None
 *
 * @ Pass/ Fail criteria        None
 *************************************************************************************************************/
void SystemDelay(UINT32 DelayCount)
{
    while ( DelayCount )
    {
        DelayCount--;
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}