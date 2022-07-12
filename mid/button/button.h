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
 * File name: button.h
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
#ifndef SOURCE_HARD_BUTTON_BUTTON_H_
#define SOURCE_HARD_BUTTON_BUTTON_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stdbool.h>
#include "typedefs.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define BUTTON_ONE_PORT				gpioPortD
#define BUTTON_ONE_PIN				(4U)

#define BUTTON_TWO_PORT				gpioPortD
#define BUTTON_TWO_PIN				(3U)

#define BUTTON_INIT					{{BUTTON_ONE_PORT,BUTTON_ONE_PIN},{BUTTON_TWO_PORT,BUTTON_TWO_PIN}}

typedef enum
{
	PRESS_1	= 1,
	PRESS_2 ,
	PRESS_3 ,
	PRESS_4 ,
	PRESS_5,
	PRESS_MAX = 10,
	HOLD_1s	,
	HOLD_2s	,
	HOLD_3s	,
	HOLD_4s	,
	HOLD_5s ,
	HOLD_MAX,
	UNKNOWN,
}ButtonState;

enum
{
	BUTTON_ONE,
	BUTTON_TWO
};

enum
{
	BUTTON_PRESS,
	BUTTON_RELEASE
}ButtonEvent;
typedef void (*pButtonHoldingEvent_t)(u8_t byIndex, ButtonState buttonHoldState);
typedef void (*pButtonPressEvent_t)(u8_t byIndex, ButtonState buttonPressState);

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
void buttonInit(pButtonHoldingEvent_t ButtonHoldingHandler,pButtonPressEvent_t ButtonPressHandler);
/******************************************************************************/
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

#endif /* SOURCE_HARD_BUTTON_BUTTON_H_ */
