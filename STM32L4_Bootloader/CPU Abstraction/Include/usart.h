/******************************************************************************
 *
 *
 *    File name   : USART.h
 *    Description : Wrapper Function for USART to communicate with Application
 *                  and Driver layer.
 *    Layer       : CPU Abstaction
 *
 *    History :
 *
 *****************************************************************************/

#ifndef _USART_H_
#define _USART_H_

/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "Initialisation.h"
/******************************************************************************
 * Defines here
 *****************************************************************************/
#define BOOTLOADER_COMM_BAUDRATE          115200
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
void USART_Initialisation(void);

/******************************************************************************
* Public Variable here (externed from)
******************************************************************************/
/* Declare the structure */
extern LL_USART_InitTypeDef SERIAL_USART_Handle;
/******************************************************************************
* Private Variables here
******************************************************************************/


#endif /*_USART_H_*/
