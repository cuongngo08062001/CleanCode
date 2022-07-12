
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
 * File name: led.c
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
#include <em_timer.h>
#include "source/mid/led/led.h"
/******************************************************************************/
/*                               DEFINITIONS                         		  */
/******************************************************************************/
typedef struct {
  GPIO_Port_TypeDef   GPIO_Port;
  u8_t		          byPin;
  bool_t 			  boLedBlinkMode;
  LedColor  		  LedColor;
  u32_t 			  byOnTime;
  u32_t			  	  byOffTime;
  u8_t			      byBlinkTime;
} LedArray_t;

#define TIMER_DIM_INIT                                                          	 \
{                                                                                  	 \
    FALSE,                 /* Enable timer when initialization completes. */          \
    FALSE,                /* Stop counter during debug halt. */                       \
    timerPrescale1,       /* No prescaling. */                                        \
    timerClkSelHFPerClk,  /* Select HFPER / HFPERB clock. */                          \
    FALSE,                /* Not 2x count mode. */                                    \
    FALSE,                /* No ATI. */                                               \
    timerInputActionNone, /* No action on falling input edge. */                      \
    timerInputActionNone, /* No action on rising input edge. */                       \
    timerModeUp,          /* Up-counting. */                                          \
    FALSE,                /* Do not clear DMA requests when DMA channel is active. */ \
    FALSE,                /* Select X2 quadrature decode mode (if used). */           \
    FALSE,                /* Disable one shot. */                                     \
    FALSE                 /* Not started/stopped/reloaded by other timers. */         \
}

/******************************************************************************/
/*                              CONTROL EVENTS                    	 		  */
/******************************************************************************/
EmberEventControl ledOneToggleEventControl,ledTwoToggleEventControl;
EmberEventControl *ledEventControl[LED_RGB_COUNT];

/******************************************************************************/
/*                              PRIVATE DATAs                    	 		  */
/******************************************************************************/
LedArray_t led_Array[LED_RGB_COUNT][LED_RGB_ELEMENT] = {LED_RGB_1, LED_RGB_2};
LedArray_t ledAction[LED_RGB_COUNT];

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void toggleLedHandle(LedNumber ledIndex);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/

/**
 * @func    ledInit
 * @brief   Initialize LED
 * @param   None
 * @retval  None
 */
void ledInit(void)
{
	CMU_ClockEnable(cmuClock_GPIO, TRUE);
	for(i8_t i = 0;i <LED_RGB_COUNT;i++)
	{
		for(i8_t j = 0; j< LED_RGB_ELEMENT;j++)
		{
			GPIO_PinModeSet(led_Array[i][j].GPIO_Port, led_Array[i][j].byPin,
							gpioModePushPull,0);
		}
	}
	turnOffRBGLed(LED_ONE);
	turnOffRBGLed(LED_TWO);
	ledEventControl[LED_ONE] =(EmberEventControl *) &ledOneToggleEventControl;
	ledEventControl[LED_TWO] =(EmberEventControl *) &ledTwoToggleEventControl;
}

/**
 * @func    turnOffRBGLed
 * @brief   Turn off number led
 * @param   ledNumber
 * @retval  None
 */
void turnOffRBGLed(LedNumber ledIndex)
{
	for(i8_t j=0;j<LED_RGB_ELEMENT;j++)
	{
		GPIO_PinOutSet(led_Array[ledIndex][j].GPIO_Port, led_Array[ledIndex][j].byPin);
	}
}

/**
 * @func    turnOnLed
 * @brief   Turn on led
 * @param   ledNumber, ledColor_e
 * @retval  None
 */
void turnOnLed(LedNumber index, LedNumber LedColor)
{
	for(i8_t j=0;j<LED_RGB_ELEMENT;j++)
	{
		if(((LedColor >> j) & 0x01) == 1)
		{
			GPIO_PinOutClear(led_Array[index][j].GPIO_Port, led_Array[index][j].byPin);
		}
		else{
			GPIO_PinOutSet(led_Array[index][j].GPIO_Port, led_Array[index][j].byPin);
		}
	}
}

/**
 * @func    toggleLed
 * @brief   toggled LED
 * @param   ledNumber, ledColor_e, toggleTime, onTimeMs, offTimeMs
 * @retval  None
 */
void toggleLed(LedNumber ledIndex, LedColor LedColor, u8_t byToggleTime, u32_t byOnTimeMs, u32_t byOffTimeMs)
{
	ledAction[ledIndex].boLedBlinkMode = LED_TOGGLE;
	ledAction[ledIndex].LedColor = LedColor;
	ledAction[ledIndex].byOnTime = byOnTimeMs;
	ledAction[ledIndex].byOffTime = byOffTimeMs;
	ledAction[ledIndex].byBlinkTime = byToggleTime*2;

	emberEventControlSetActive(*ledEventControl[ledIndex]);
}

/**
 * @func    toggleLedHandle
 * @brief   Event Led Handler
 * @param   ledNumber
 * @retval  None
 */
void toggleLedHandle(LedNumber ledIndex)
{
	if(ledAction[ledIndex].byBlinkTime !=0)
	{
		if(ledAction[ledIndex].byBlinkTime % 2)
		{
			for( i8_t i = 0; i<LED_RGB_ELEMENT; i++)
			{
				if(((ledAction[ledIndex].LedColor >> i ) & 0x01) == 1 )
				{
					GPIO_PinOutClear(led_Array[ledIndex][i].GPIO_Port, led_Array[ledIndex][i].byPin);
				}
				else
				{
					GPIO_PinOutSet(led_Array[ledIndex][i].GPIO_Port, led_Array[ledIndex][i].byPin);
				}
			}
			emberEventControlSetDelayMS(*ledEventControl[ledIndex], ledAction[ledIndex].byOnTime);
		}else
		{
			for( i8_t j = 0; j<LED_RGB_ELEMENT; j++)
			{
				GPIO_PinOutSet(led_Array[ledIndex][j].GPIO_Port, led_Array[ledIndex][j].byPin);
			}
			emberEventControlSetDelayMS(*ledEventControl[ledIndex], ledAction[ledIndex].byOffTime);
		}
		ledAction[ledIndex].byBlinkTime--;
	}
	else
	{
		ledAction[ledIndex].boLedBlinkMode = LED_FREE;
		for( i8_t j = 0; j<LED_RGB_ELEMENT; j++)
		{
			GPIO_PinOutSet(led_Array[ledIndex][j].GPIO_Port, led_Array[ledIndex][j].byPin);
		}
	}
}

/**
 * @func    LED_ONEToggleEventHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
void ledOneToggleEventHandler(void)
{
	emberEventControlSetInactive(ledOneToggleEventControl);
	switch(ledAction[LED_ONE].boLedBlinkMode)
	{
	case LED_TOGGLE:
		toggleLedHandle(LED_ONE);
		break;
	default:
		break;
	}
}


/**
 * @func    LED_TWOToggleEventHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
void ledTwoToggleEventHandler(void)
{
	emberEventControlSetInactive(ledTwoToggleEventControl);
	switch(ledAction[LED_TWO].boLedBlinkMode)
	{
	case LED_TOGGLE:
		toggleLedHandle(LED_TWO);
		break;
	default:
		break;
	}
}



