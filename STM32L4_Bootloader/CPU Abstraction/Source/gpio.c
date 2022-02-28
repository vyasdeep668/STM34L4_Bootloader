/******************************************************************************
 *
 *
 *    File name   : gpio.c
 *    Description : Wrapper Function for GPIO to communicate with Application
 *                  and Driver layer.
 *    Layer       : CPU Abstraction
 *
 *    History :
 *
 *****************************************************************************/


/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "initialisation.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g071xx.h"
#include "gpio.h"
#include "main.h"
#include "typedef.h"

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
UINT16 g_u16ServiceLEDScaler;

/******************************************************************************
* Private Variables here
******************************************************************************/
BOOL bLED_Status;

/*************************************************************************************************************
 *
 * @Function		  	    void GPIO_Initialise(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This function initialises  GPIO
 *
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void GPIO_Initialise(void)
{
    /* Create Structure */
    GPIO_InitTypeDef GPIO_InitStructure;
    /***************Secure PRNT***********
    *             GPIO C
    *     GPIO_PIN_15             SERVICE LED

                  GPIO D
    *     GPIO_PIN_4              USART2 DIR
    *     GPIO_PIN_5              USART2 TX
    *     GPIO_PIN_6              USART2 RX
    ***************************************/

    /*Initialise GPIOC OUTPUT for Service LED */
    GPIO_InitStructure.Pin   = SERVICE_LED_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;

    /* Initialise GPIO with Above Values */
    HAL_GPIO_Init(SERVICE_LED_PORT, &GPIO_InitStructure);


    /*Initialise GPIOD OUTPUT for USART DIR */
    GPIO_InitStructure.Pin   = SERIAL_DIR_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;

    /* Initialise GPIO with Above Values */
    HAL_GPIO_Init(SERIAL_DIR_PORT,&GPIO_InitStructure);


    /*-----------------USART 2---------------*/

    /* USART 2 GPIO Configuration */
    GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = GPIO_AF0_USART2;

    /* USART 2 Tx Pin Init */
    GPIO_InitStructure.Pin   = SERIAL_TX;
    HAL_GPIO_Init(SERIAL_PORT, &GPIO_InitStructure);

    /* USART 2 Rx Pin Init */
    GPIO_InitStructure.Pin   = SERIAL_RX;
    HAL_GPIO_Init(SERIAL_PORT, &GPIO_InitStructure);

}

/*************************************************************************************************************
 *
 * @Function		  	    void GPIO_ServiceLedHandler(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        Service Led Handler
 *
 * @param[in]               None
 *
*
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void GPIO_ServiceLedHandler(void)
{
    if( g_u16ServiceLEDScaler == 0)
    {
        GPIO_ServiceToogle();
        g_u16ServiceLEDScaler = SERVICE_LED_TOGGLE_RATE;
    }
    else
    {
    }
}

/*************************************************************************************************************
 *
 * @Function		  	    void GPIO_ServiceToogle(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This Function Toogles Service LED pin
 *
 * @param[in]               None
 *
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void GPIO_ServiceToogle(void)
{
    if ( bLED_Status == GPIO_PIN_SET )
    {
        SERVICE_LED_OFF;
        bLED_Status = GPIO_PIN_RESET;
    }
    else
    {
        SERVICE_LED_ON;
        bLED_Status = GPIO_PIN_SET;
    }
}