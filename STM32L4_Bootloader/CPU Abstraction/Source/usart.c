/******************************************************************************
 *
 *
 *    File name   : USART.c
 *    Description : Wrapper Function for USART to communicate with Application
 *                  and Driver layer.
 *    Layer       : CPU Abstaction
 *
 *    History :
 *
 *****************************************************************************/


/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "Initialisation.h"
#include "usart.h"
/******************************************************************************
 * Defines here
 *****************************************************************************/

/******************************************************************************
* Enums here
******************************************************************************/

/******************************************************************************
* Structs here
******************************************************************************/
/* Declare the structure */
LL_USART_InitTypeDef SERIAL_USART_Handle;
LL_USART_ClockInitTypeDef SERIAL_USART_CLOCK_Handle;
/******************************************************************************
* Public function prototypes (externed from)
******************************************************************************/

/******************************************************************************
* Private function prototypes
******************************************************************************/

/******************************************************************************
* Public Variable here (externed from)
******************************************************************************/

/******************************************************************************
* Private Variables here
******************************************************************************/

/*************************************************************************************************************
 *
 * @Function		  	void USART_Initialisation(void)
 *
 * @Created By 	  		Deep Vyas
 *
 * @Brief		  	Initialise USART
 *                              No parity
 *                              BoudRate 115200
 *                              StopBit 1
 *                              Hardware Flow control None
 *                              Wordlength 8bit
 * @param[in]                   None
 * @param[out]                  None
 *
 * @Return		        None
 *
 * @Pass/Fail criteria          None
 *************************************************************************************************************/
void USART_Initialisation(void)
{
    /*Configure USART Clock*/
    LL_USART_ClockStructInit(&SERIAL_USART_CLOCK_Handle);
    LL_USART_ClockInit(SERIAL_UART, &SERIAL_USART_CLOCK_Handle);

    /*Configure The USART*/
    SERIAL_USART_Handle.PrescalerValue = LL_USART_PRESCALER_DIV1;
    SERIAL_USART_Handle.BaudRate = BOOTLOADER_COMM_BAUDRATE;
    SERIAL_USART_Handle.DataWidth = LL_USART_DATAWIDTH_8B;
    SERIAL_USART_Handle.StopBits = LL_USART_STOPBITS_1;
    SERIAL_USART_Handle.Parity = LL_USART_PARITY_NONE;
    SERIAL_USART_Handle.TransferDirection = LL_USART_DIRECTION_TX_RX;
    SERIAL_USART_Handle.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    SERIAL_USART_Handle.OverSampling = LL_USART_OVERSAMPLING_16;

    /* SERIAL USART Init */
    LL_USART_Init(SERIAL_UART, &SERIAL_USART_Handle);

    /* Enable/Disable the required USART interrupts */
    LL_USART_EnableIT_RXNE_RXFNE(SERIAL_UART);
    LL_USART_DisableIT_TXE_TXFNF(SERIAL_UART);

    LL_USART_DisableIT_IDLE(SERIAL_UART);
    LL_USART_DisableIT_TC(SERIAL_UART);
    LL_USART_DisableIT_PE(SERIAL_UART);
    LL_USART_DisableIT_CM(SERIAL_UART);
    LL_USART_DisableIT_RTO(SERIAL_UART);
    LL_USART_DisableIT_EOB(SERIAL_UART);
    LL_USART_DisableIT_ERROR(SERIAL_UART);

    /* Enable USART */
    LL_USART_Enable(SERIAL_UART);

}