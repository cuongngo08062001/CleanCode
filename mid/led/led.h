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
#include "source/utils/typedefs.h"
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
enum{
    LED_OFF = 0x00,
    LED_ON                      ,
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
/**
 * @func    ledInit
 * @brief   Initialize LED
 * @param   None
 * @retval  None
 */
void ledInit(void);
/**
 * @func    turnOffRBGLed
 * @brief   Turn off number led
 * @param   LedNumber ledIndex (index of Led)
 * @retval  None
 */
void turnOffRBGLed(LedNumber ledIndex);

/**
 * @func    turnOnLed
 * @brief   Turn on led
 * @param   LedNumber index  (index of Led)
 *          LedNumber LedColor (Color of Led: Green, Blue, Red, Pink, Yellow)
 * @retval  None
 */
void turnOnLed(LedNumber index, LedNumber LedColor);
/**
 * @func    toggleLed
 * @brief   toggled LED
 * @param   LedNumber ledIndex, (index of Led)
 *          LedColor LedColor, (Color of Led: Green, Blue, Red, Pink, Yellow)
 *          u8_t byToggleTime, (Times of Toggle Led)
 *          u32_t byOnTimeMs,
 *          u32_t byOffTimeMs
 * @retval  None
 */
void toggleLed(LedNumber ledIndex, LedColor LedColor, u8_t byToggleTime, u32_t byOnTimeMs, u32_t byOffTimeMs);
#endif /* SOURCE_MID_LED_H_ */
