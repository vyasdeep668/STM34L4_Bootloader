#ifndef _INITIALISATION_H_
#define _INITIALISATION_H_

/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "stm32g071xx.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_rcc.h"
#include "stm32g0xx_hal_rcc_ex.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_tim.h"
#include "stm32g0xx_hal_usart.h"
#include "stm32g0xx_hal_cortex.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_hal_flash.h"

#include "typedef.h"
#include "gpio.h"
#include "timer.h"
#include "usart.h"
#include "bootloader.h"
#include "Communication.h"
#include "main.h"
#include "rsa.h"
#include "flash.h"
#include "FirmwareSignature_SHA256.h"
#include "Bootloader_Proprietary_Encryption.h"
/******************************************************************************
 * Defines here
 *****************************************************************************/
#define SERIAL_UART                             USART2
#define SERVICE_LED_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define SERIAL_UART_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
#define SERIAL_UART_DIR_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()
#define SERIAL_UART_CLK_ENABLE()                __HAL_RCC_USART2_CLK_ENABLE()
#define TIMER_CLK_ENABLE()                      __HAL_RCC_TIM2_CLK_ENABLE()
#define TIMER_IRQ                               TIM2_IRQn
#define SERIAL_IRQ                              USART2_IRQn
#define SERIAL_USART_IRQHandler                 USART2_IRQHandler



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
void DeviceInitialise(void);
void ClockInitialise(void);
void PeripheralInitialise(void);
void NVIC_Configuration(void);
void NVIC_DisableInterrupt(void);
/******************************************************************************
* Public Variable here (externed from)
******************************************************************************/

/******************************************************************************
* Private Variables here
******************************************************************************/

#endif /*_INITIALISATION_H_*/
