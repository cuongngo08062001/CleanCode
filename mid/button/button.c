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
#include <app/framework/include/af.h>
#include "source/mid/button/button.h"
/******************************************************************************/
/*                              CONTROL EVENTS                    	 		  */
/******************************************************************************/

EmberEventControl buttonPressAndHoldEventControl;
EmberEventControl buttonReleaseEventControl;
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define BUTTON_COUNT					2
#define GPIO_DOUT						0
#define BUTTON_DEBOUNCE					5
#define BUTTON_CHECK_HOLD_CYCLES_MS		200
#define BUTTON_CHECK_RELEASE_MS			500

#define HOLD_TIME_MIN					5

typedef struct {
  GPIO_Port_TypeDef		GPIO_Port;
  unsigned int			byPin;
  bool_t				boState;
  u8_t					byPressCount;
  u8_t					byHoldTime;
  bool_t				boRelease;
  bool_t				boPress;
  bool_t               boIsHolding;
} Buttonx_t;

/******************************************************************************/
/*                              PRIVATE DATAs                                 */
/******************************************************************************/
pButtonHoldingEvent_t ButtonHoldingCallbackFunc = NULL;
pButtonPressEvent_t ButttonPressAndHoldingCallbackFunc = NULL;
Buttonx_t buttonArray[BUTTON_COUNT] = BUTTON_INIT;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
/******************************************************************************/
/*                              EXPORTED FUNTIONS                             */
/******************************************************************************/
/******************************************************************************/
/*                              PRIVATE FUNTIONS                              */
/******************************************************************************/
static void buttonPressAndHoldEventHandle(void);
static void buttonReleaseEventHandle(void);
static void halInternalButtonIsr(u8_t pin);
static u8_t getButtonIndex(u8_t pin);
static void resetButtonParameter(u8_t byIndex);
/******************************************************************************/

/*
 * @func	buttonInit
 * @brief	Initialize Button
 * @param	ButtonHoding, ButtonPress
 * @retval	None
 */
void buttonInit(pButtonHoldingEvent_t ButtonHoding,pButtonPressEvent_t ButtonPress)
{
  GPIOINT_Init();

  CMU_ClockEnable(cmuClock_GPIO, TRUE);
  u8_t i;
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
                      TRUE,
                      TRUE,
                      TRUE);
  }

  ButtonHoldingCallbackFunc=ButtonHoding;
  ButttonPressAndHoldingCallbackFunc=ButtonPress;

}
/*
 * @func	halInternalButtonIsr
 * @brief	IRQ button
 * @param	pin
 * @retval	None
 */
void halInternalButtonIsr(u8_t byPin)
{
  u8_t buttonStateNow;
  u8_t buttonStatePrev;
  u32_t debounce;
  u8_t byButtonIndex;

  byButtonIndex = getButtonIndex(byPin);
  // check valid byIndex
  if(byButtonIndex==-1)
	  return;

  buttonStateNow = GPIO_PinInGet(buttonArray[byButtonIndex].GPIO_Port, buttonArray[byButtonIndex].byPin);
  for ( debounce = 0;
        debounce < BUTTON_DEBOUNCE;
        debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0 ) {
    buttonStatePrev = buttonStateNow;
    buttonStateNow = GPIO_PinInGet(buttonArray[byButtonIndex].GPIO_Port, buttonArray[byButtonIndex].byPin);
  }

  buttonArray[byButtonIndex].boState = buttonStateNow;

  if(buttonStateNow == BUTTON_PRESS)
  {
	  buttonArray[byButtonIndex].byPressCount++;
	  if(buttonArray[byButtonIndex].boPress != TRUE)
	  {
		  emberEventControlSetActive(buttonPressAndHoldEventControl);
	  }

	  buttonArray[byButtonIndex].boIsHolding=FALSE;
	  buttonArray[byButtonIndex].byHoldTime=0;
	  buttonArray[byButtonIndex].boPress = TRUE;
	  buttonArray[byButtonIndex].boRelease = FALSE;

  }
  else
  {

	  buttonArray[byButtonIndex].boRelease = TRUE;
	  buttonArray[byButtonIndex].boPress = FALSE;
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
	bool_t boHoldTrigger =FALSE;
	for(u8_t i=0; i < BUTTON_COUNT; i++)
	{
		if(buttonArray[i].boPress ==TRUE)
		{
			boHoldTrigger = TRUE;
			buttonArray[i].byHoldTime++;
			if(buttonArray[i].byHoldTime>=5)
			{
				buttonArray[i].boIsHolding=TRUE;
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
	if(boHoldTrigger == TRUE)
		emberEventControlSetDelayMS(buttonPressAndHoldEventControl,BUTTON_CHECK_HOLD_CYCLES_MS);
}

/*
 * @func	buttonReleaseEventHandle
 * @brief	Event Button Handler
 * @param	button, ButtonHodingr
 * @retval	None
 */
void buttonReleaseEventHandler(void)
{
	emberEventControlSetInactive(buttonReleaseEventControl);
	for(u8_t i=0; i<BUTTON_COUNT; i++)
	{
		if(buttonArray[i].boRelease == TRUE)
		{
			if(ButttonPressAndHoldingCallbackFunc != NULL)
			{
				if(buttonArray[i].boIsHolding==FALSE)
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
 * @param	byIndex
 * @retval	None
 */
static void resetButtonParameter(u8_t byIndex)
{
	buttonArray[byIndex].byHoldTime = 0;
	buttonArray[byIndex].byPressCount	= 0;
	buttonArray[byIndex].boPress = FALSE;
	buttonArray[byIndex].boRelease = FALSE;
	buttonArray[byIndex].boState = 0;
	buttonArray[byIndex].boIsHolding = FALSE;
}

/*
 * @func	getButtonbyIndex
 * @brief	get number button
 * @param	pin
 * @retval	None
 */
static u8_t getButtonIndex(u8_t byPin)
{
	for(u8_t i=0; i < BUTTON_COUNT; i++)
	{
		if(buttonArray[i].byPin == byPin)
			return i;
	}
	return -1;
}

