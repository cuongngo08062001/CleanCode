/*******************************************************************************
/*******************************************************************************
 *				 _ _                                             _ _
				|   |                                           (_ _)
				|   |        _ _     _ _   _ _ _ _ _ _ _ _ _ _   _ _
				|   |       |   |   |   | |    _ _     _ _    | |   |
				|   |       |   |   |   | |   |   |   |   |   | |   |
				|   |       |   |   |   | |   |   |   |   |   | |   |
				|   |_ _ _  |   |_ _|   | |   |   |   |   |   | |   |
				|_ _ _ _ _| |_ _ _ _ _ _| |_ _|   |_ _|   |_ _| |_ _|
								(C)2022 Lumi
 * Copyright (c) 2022
 * Lumi, JSC.
 * All Rights Reserved
 *
 * File name: button.c
 *
 * Description:
 *
 * Author: CuongNV
 *
 * Last Changed By:  $Author: cuongnv $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $Jun 7, 2022
 ******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <source/mid/button/button.h>
#include "app/framework/include/af.h"

/******************************************************************************/
/*                              CONTROL EVENTS                    	 		  */
/******************************************************************************/

EmberEventControl buttonPressAndHoldEventControl;
EmberEventControl buttonReleaseEventControl;

/******************************************************************************/
/*                              PRIVATE DATAs                                 */
/******************************************************************************/
BUTTON_holdingEvent_t ButtonHoldingCallbackFunc = NULL;
BUTTON_pressEvent_t ButttonPressAndHoldingCallbackFunc = NULL;
Buttonx_t buttonArray[BUTTON_COUNT] = BUTTON_INIT;

/******************************************************************************/
/*                              EXPORTED FUNTIONS                             */
/******************************************************************************/
/******************************************************************************/
/*                              PRIVATE FUNTIONS                              */
/******************************************************************************/
void buttonPressAndHoldEventHandle(void);
void buttonReleaseEventHandle(void);
static void 	halInternalButtonIsr(uint8_t pin);
static uint8_t 	getButtonIndex(uint8_t pin);
static void 	resetButtonParameter(uint8_t index);

/******************************************************************************/
/*
 * @func	buttonInit
 * @brief	Initialize Button
 * @param	holdingHandle, pressHandler
 * @retval	None
 */
void buttonInit(BUTTON_holdingEvent_t holdingHandle,BUTTON_pressEvent_t pressHandler)
{
  GPIOINT_Init();

  CMU_ClockEnable(cmuClock_GPIO, true);
  uint8_t i;
  for ( i = 0; i < BUTTON_COUNT; i++ ) {
    /* Configure pin as input */
    GPIO_PinModeSet(buttonArray[i].GPIO_Port,
                    buttonArray[i].byPin,
					gpioModeInput,
					GPIO_DOUT);
    /* Register callbacks before setting up and enabling pin interrupt. */
    GPIOINT_CallbackRegister(buttonArray[i].byPin,
                             halInternalButtonIsr);
    /* Set rising and falling edge interrupts */
    GPIO_ExtIntConfig(buttonArray[i].GPIO_Port,
                      buttonArray[i].byPin,
                      buttonArray[i].byPin,
                      true,
                      true,
                      true);
  }

  ButtonHoldingCallbackFunc=holdingHandle;
  ButttonPressAndHoldingCallbackFunc=pressHandler;

}
/*
 * @func	halInternalButtonIsr
 * @brief	IRQ button
 * @param	pin
 * @retval	None
 */
void halInternalButtonIsr(uint8_t pin)
{
  uint8_t buttonStateNow;
  uint8_t buttonStatePrev;
  uint32_t debounce;
  uint8_t buttonIndex;

  buttonIndex = getButtonIndex(pin);
  // check valid index
  if(buttonIndex==-1)
	  return;

  buttonStateNow = GPIO_PinInGet(buttonArray[buttonIndex].GPIO_Port, buttonArray[buttonIndex].byPin);
  for ( debounce = 0;
        debounce < BUTTON_DEBOUNCE;
        debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0 ) {
    buttonStatePrev = buttonStateNow;
    buttonStateNow = GPIO_PinInGet(buttonArray[buttonIndex].GPIO_Port, buttonArray[buttonIndex].byPin);
  }

  buttonArray[buttonIndex].boState = buttonStateNow;

  if(buttonStateNow == BUTTON_PRESS)
  {
	  buttonArray[buttonIndex].byPressCount++;
	  if(buttonArray[buttonIndex].boPress != true)
	  {
		  emberEventControlSetActive(buttonPressAndHoldEventControl);
	  }

	  buttonArray[buttonIndex].boIsHolding=false;
	  buttonArray[buttonIndex].byHoldTime=0;
	  buttonArray[buttonIndex].boPress = true;
	  buttonArray[buttonIndex].boRelease = false;

  }
  else
  {

	  buttonArray[buttonIndex].boRelease = true;
	  buttonArray[buttonIndex].boPress = false;
	  emberEventControlSetInactive(buttonReleaseEventControl);
	  emberEventControlSetDelayMS(buttonReleaseEventControl,BUTTON_CHECK_RELEASE_MS);
  }
}

/*
 * @func	buttonPressAndHoldEventHandle
 * @brief	Event Button Handler
 * @param	None
 * @retval	None
 */
void buttonPressAndHoldEventHandler(void)
{
	emberEventControlSetInactive(buttonPressAndHoldEventControl);
	bool holdTrigger =false;
	for(int i=0; i<BUTTON_COUNT; i++)
	{
		if(buttonArray[i].boPress ==true)
		{
			holdTrigger = true;
			buttonArray[i].byHoldTime++;
			if(buttonArray[i].byHoldTime>=5)
			{
				buttonArray[i].boIsHolding=true;
				buttonArray[i].byPressCount=0;
			}

			if(ButtonHoldingCallbackFunc != NULL)
			{
				if((buttonArray[i].byHoldTime %5)==0)
				{
					ButtonHoldingCallbackFunc(i,buttonArray[i].byHoldTime/5 + PRESS_MAX);
				}
			}
		}
	}
	if(holdTrigger == true)
		emberEventControlSetDelayMS(buttonPressAndHoldEventControl,BUTTON_CHECK_HOLD_CYCLES_MS);
}

/*
 * @func	buttonReleaseEventHandle
 * @brief	Event Button Handler
 * @param	button, holdingHandler
 * @retval	None
 */
void buttonReleaseEventHandler(void)
{
	emberEventControlSetInactive(buttonReleaseEventControl);
	for(int i=0; i<BUTTON_COUNT; i++)
	{
		if(buttonArray[i].boRelease == true)
		{
			if(ButttonPressAndHoldingCallbackFunc != NULL)
			{
				if(buttonArray[i].boIsHolding==false)
				{
					ButttonPressAndHoldingCallbackFunc(i, buttonArray[i].byPressCount >= PRESS_MAX ? UNKNOWN:buttonArray[i].byPressCount);
				}else
				{
					ButtonHoldingCallbackFunc(i, (buttonArray[i].byHoldTime/5 + PRESS_MAX) >= HOLD_MAX ? UNKNOWN :(buttonArray[i].byHoldTime/5 + PRESS_MAX));
				}
			}

			resetButtonParameter(i);
		}
	}
}

/*
 * @func	resetButtonParameter
 * @brief	Reset parameter
 * @param	index
 * @retval	None
 */
static void resetButtonParameter(uint8_t index)
{
	buttonArray[index].byHoldTime = 0;
	buttonArray[index].byPressCount	= 0;
	buttonArray[index].boPress = false;
	buttonArray[index].boRelease = false;
	buttonArray[index].boState = 0;
	buttonArray[index].boIsHolding = false;
}

/*
 * @func	getButtonIndex
 * @brief	get number button
 * @param	pin
 * @retval	None
 */
static uint8_t getButtonIndex(uint8_t pin)
{
	for(int i=0; i<BUTTON_COUNT; i++)
	{
		if(buttonArray[i].byPin == pin)
			return i;
	}
	return -1;
}

