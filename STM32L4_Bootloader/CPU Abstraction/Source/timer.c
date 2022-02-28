/******************************************************************************
 *
 *
 *    File name   : Timer.c
 *    Description : Wrapper Function for Timer Initialaisation
 *
 *    Layer       : CPU Abstraction
 *
 *    History :
 *
 *    $Revision: 0 $
 *****************************************************************************/


/******************************************************************************
 * File includes here
 *****************************************************************************/
#include "Initialisation.h"
#include "timer.h"

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
TIM_HandleTypeDef TIMER_Handle;
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
 * @Function		  	    void TIM2_Initialise(void)
 *
 * @Created By 	  		    Deep Vyas
 *
 * @Brief		  	        This function initialises the Timer 2 counting as a upcounter.
 *                              Timer Update Event = TimerClk/(PSC+1)*(ARR+1)*(RCR+1)
 *                              Timer Configured for 1ms
 *                              Timer Clock Division = System CLK/1 = 8Mhz
 *                              Counter Mode = Upcounting
 *                              Period(ARR) = Auto Reload Register = 3999
 *                              Prescaler(PSC) = 1
 *
 * @param[in]               None
 * @param[out]              None
 *
 * @Return		            None
 *
 * @Pass/Fail criteria      None
 *************************************************************************************************************/
void TIM2_Initialisation(void)
{
      /*Configure The Timer*/
      TIMER_Handle.Instance = TIM2;
      TIMER_Handle.Init.Prescaler = TIM2_PSC;
      TIMER_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
      TIMER_Handle.Init.Period = TIM2_PERIOD;
      TIMER_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      TIMER_Handle.Init.RepetitionCounter = 0;

      /* SERIAL USART Init */
      HAL_TIM_Base_Init(&TIMER_Handle);

      /* Enable Timer in Interrupt Mode*/
      HAL_TIM_Base_Start_IT(&TIMER_Handle);
}

