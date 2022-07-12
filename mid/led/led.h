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
 * File name: led.h
 *
 * Description:
 *
 * Author: CuongNV
 *
 * Last Changed By:  $Author: cuongnv $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $Jun 7, 2022
 ******************************************************************************/
// Enclosing macro to prevent multiple inclusion
#ifndef SOURCE_MID_LED_H_
#define SOURCE_MID_LED_H_
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "typedefs.h"
/******************************************************************************/
/*                       EXPORTED TYPES & DEFINITIONS            		      */
/******************************************************************************/
#define LED_RGB_COUNT				2
#define LED_RGB_ELEMENT				3

#define LED_RGB_DIM_T				10000

#define LED_PORT_1					gpioPortA
#define LED_BLUE_PIN_1				(0U)
#define LED_GREEN_PIN_1				(3U)
#define LED_RED_PIN_1				(4U)

#define LED_PORT_2					gpioPortD
#define LED_BLUE_PIN_2				(2U)
#define LED_GREEN_PIN_2				(1U)
#define LED_RED_PIN_2				(0U)

#define LED_RGB_1                       { { LED_PORT_1, LED_RED_PIN_1 }, { LED_PORT_1, LED_GREEN_PIN_1 }, { LED_PORT_1, LED_BLUE_PIN_1 } }
#define LED_RGB_2                       { { LED_PORT_2, LED_RED_PIN_2 }, { LED_PORT_2, LED_GREEN_PIN_2 }, { LED_PORT_2, LED_BLUE_PIN_2 } }


typedef enum{
	LED_ONE = 0x00,
	LED_TWO
}LedNumber;
typedef enum
{
	LED_OFF 			= 0x00,
	LED_ON						,
	LED_RED				= BIT(0),
	LED_GREEN			= BIT(1),
	LED_BLUE			= BIT(2),
	LED_PINK			= LED_RED  | LED_BLUE,
	LED_YELLOW			= LED_GREEN| LED_RED,
	LED_RGB				= LED_RED  | LED_GREEN | LED_BLUE
}LedColor;

typedef enum
{
	RED = 0x00,
	GREEN,
	BLUE,
	OFF
}LedState;
enum{
	LED_FREE = 0x00,
	LED_TOGGLE
};

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/
void ledInit(void);
void turnOffRBGLed(LedNumber ledIndex);
void turnOnLed(LedNumber index, LedNumber LedColor);
void toggleLed(LedNumber ledIndex, LedColor LedColor, i8_t byToggleTime, i32_t byOnTimeMs, i32_t byOffTimeMs);
#endif /* SOURCE_MID_LED_H_ */
