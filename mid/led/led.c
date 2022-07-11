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
#include <source/mid/led/led.h>
#include "app/framework/include/af.h"
#include "em_timer.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
typedef struct {
  GPIO_Port_TypeDef   GPIO_Port;
  unsigned int        byPin;
  bool 				  boLedBlinkMode;
  LedColor_e		  LedColor;
  uint32_t 			  byOnTime;
  uint32_t			  byOffTime;
  uint8_t			  byBlinkTime;
} LedArray_t;
/******************************************************************************/
/*                              CONTROL EVENTS                    	 		  */
/******************************************************************************/
EmberEventControl led1ToggleEventControl,led2ToggleEventControl;
EmberEventControl *ledEventControl[LED_RGB_COUNT];
/******************************************************************************/
/*                              PRIVATE DATAs                    	 		  */
/******************************************************************************/
LedArray_t led_Array[LED_RGB_COUNT][LED_RGB_ELEMENT] = {LED_RGB_1, LED_RGB_2};
LedArray_t ledAction[LED_RGB_COUNT];

/******************************************************************************/
/**
 * @func    ledInit
 * @brief   Initialize LED
 * @param   None
 * @retval  None
 */
void ledInit(void)
{
	CMU_ClockEnable(cmuClock_GPIO, true);
	for(int i = 0;i <LED_RGB_COUNT;i++)
	{
		for(int j = 0; j< LED_RGB_ELEMENT;j++)
		{
			GPIO_PinModeSet(led_Array[i][j].GPIO_Port, led_Array[i][j].byPin,
							gpioModePushPull,0);
		}
	}
	turnOffRBGLed(LED1);
	turnOffRBGLed(LED2);
	ledEventControl[LED1] =(EmberEventControl *) &led1ToggleEventControl;
	ledEventControl[LED2] =(EmberEventControl *) &led2ToggleEventControl;
}

/**
 * @func    turnOffRBGLed
 * @brief   Turn off number led
 * @param   ledNumber
 * @retval  None
 */
void turnOffRBGLed(LedNumber_e ledIndex)
{
	for(int j=0;j<LED_RGB_ELEMENT;j++)
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
void turnOnLed(LedNumber_e index, LedColor_e ledColor)
{
	for(int j=0;j<LED_RGB_ELEMENT;j++)
	{
		if(((ledColor >> j) & 0x01) == 1)
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
void toggleLed(LedNumber_e ledIndex, LedColor_e color, uint8_t byToggleTime, uint32_t byOnTimeMs, uint32_t byOffTimeMs)
{
	ledAction[ledIndex].boLedBlinkMode = LED_TOGGLE;
	ledAction[ledIndex].LedColor = color;
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
void toggleLedHandle(LedNumber_e ledIndex)
{
	if(ledAction[ledIndex].byBlinkTime !=0)
	{
		if(ledAction[ledIndex].byBlinkTime % 2)
		{
			for( int i = 0; i<LED_RGB_ELEMENT; i++)
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
			for( int j = 0; j<LED_RGB_ELEMENT; j++)
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
		for( int j = 0; j<LED_RGB_ELEMENT; j++)
		{
			GPIO_PinOutSet(led_Array[ledIndex][j].GPIO_Port, led_Array[ledIndex][j].byPin);
		}
	}
}

/**
 * @func    led1ToggleEventHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
void led1ToggleEventHandler(void)
{
	emberEventControlSetInactive(led1ToggleEventControl);
	switch(ledAction[LED1].boLedBlinkMode)
	{
	case LED_TOGGLE:
		toggleLedHandle(LED1);
		break;
	default:
		break;
	}
}


/**
 * @func    led2ToggleEventHandle
 * @brief   Event Led Handler
 * @param   None
 * @retval  None
 */
void led2ToggleEventHandler(void)
{
	emberEventControlSetInactive(led2ToggleEventControl);
	switch(ledAction[LED2].boLedBlinkMode)
	{
	case LED_TOGGLE:
		toggleLedHandle(LED2);
		break;
	default:
		break;
	}
}



