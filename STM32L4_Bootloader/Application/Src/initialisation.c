/******************************************************************************
 *
 *
 *    File name   : initialaisation.c
 *    Description : Initialaise all the GPIO Clock and Timer Module
 *    Layer       : Appplication
 *
 *    History :
 *
 *****************************************************************************/


/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "Initialisation.h"
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
void SystemClock_Config(void);

/******************************************************************************
* Public Variable here (externed from)
******************************************************************************/



/******************************************************************************
* Private Variables here
******************************************************************************/

/*************************************************************************************************************
 *
 * @Function		  	    void DeviceInitialise(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This function initialises different peripherals like,
 *                          Clock , GPIO etc.
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void DeviceInitialise(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /*Clock Init for GPIO and Other Peripherals*/
    ClockInitialise();

    /*Peripheral Initialise */
    PeripheralInitialise();

    /* Reset communication variables */
    ResetCommunicationVariables();
}

/*************************************************************************************************************
 *
 * @Function		        void PeripheralInitialise(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This function initialises different peripherals like,
 *                          GPIO Timer NVIC etc.
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void PeripheralInitialise(void)
{
    GPIO_Initialise();
    NVIC_Configuration();
    USART_Initialisation();
    TIM2_Initialisation();
}


/*************************************************************************************************************
 *
 * @Function		  	        void SystemClock_Config(void)
 *
 * @Created By 	  		        Deep Vyas
 *
 * @Brief		                Initializes the RCC Oscillators according to the specified parameters
 *                                      in the RCC_OscInitTypeDef structure. Initializes the CPU, AHB and APB
 *                                      buses clocks
 *
 * @param[in]                           None
 * @param[out]                          None
 *
 * @Return		                None
 *
 * @Pass/Fail criteria                  None
 *************************************************************************************************************/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  RCC_PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}
/*************************************************************************************************************
 *
 * @Function		  	    void NVIC_Configuration(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		          	This function defines the interrupt priority for different peripherals like,
 *                          Timer and Serial.
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void NVIC_Configuration(void)
{
   /* Configure TIM2 Interrupt */
   HAL_NVIC_SetPriority(SERIAL_IRQ, 1, 0);
   HAL_NVIC_EnableIRQ(SERIAL_IRQ);

   /*Configure USART Interrupt*/
   HAL_NVIC_SetPriority(TIMER_IRQ, 0, 0);
   HAL_NVIC_EnableIRQ(TIMER_IRQ);

}

/*************************************************************************************************************
 *
 * @Function		        void Clock_Initialise(void)
 *
 * @Created By 	  	        Deep Vyas
 *
 * @Brief		  	        This function initialises different peripherals like,
 *                          GPIO etc.
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void ClockInitialise(void)
{
  /*Initialise Clock for GPIOB (UART TX/RX) */
  SERIAL_UART_GPIO_CLK_ENABLE();

  /*Initialise Clock for GPIOD (UART DIR)*/
  SERIAL_UART_DIR_GPIO_CLK_ENABLE();

  /*Enable Clock for Serial Uart 1*/
  SERIAL_UART_CLK_ENABLE();

  /*Port C Clock Initialize for Service LED*/
  SERVICE_LED_GPIO_CLK_ENABLE();

  /*Timer 2 Clock Initialise*/
  TIMER_CLK_ENABLE();

}

/*************************************************************************************************************
 *
 * @Function		        void NVIC_DisableInterrupt(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This function disables the interrupt priority for different peripherals like,
 *                          Timer and Serial.
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void NVIC_DisableInterrupt(void)
{
   /* Configure TIM2 Interrupt */
   HAL_NVIC_SetPriority(SERIAL_IRQ, 1, 0);
   HAL_NVIC_DisableIRQ(SERIAL_IRQ);

   /*Configure USART Interrupt*/
   HAL_NVIC_SetPriority(TIMER_IRQ, 0, 0);
   HAL_NVIC_DisableIRQ(TIMER_IRQ);
}


