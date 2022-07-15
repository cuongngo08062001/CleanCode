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
#include "source/utils/typedefs.h"
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
	PRESS_ONE_TIME	= 1,
	PRESS_TWO_TIMES ,
	PRESS_THREE_TIMES ,
	PRESS_FOUR_TIMES ,
	PRESS_FIVE_TIMES,
	PRESS_MAX = 10,
	HOLD_ONE_SECOND	,
	HOLD_TWO_SECONDs,
	HOLD_THREE_SECONDs,
	HOLD_FOUR_SECONDs,
	HOLD_FIVE_SECONDs,
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
/*
 * @func    buttonInit
 * @brief   Initialize Button
 * @param   pButtonHoldingEvent_t ButtonHoding
 *          pButtonPressEvent_t ButtonPress
 * @retval  None
 */
void buttonInit(pButtonHoldingEvent_t ButtonHoldingHandler,pButtonPressEvent_t ButtonPressHandler);

#endif /* SOURCE_HARD_BUTTON_BUTTON_H_ */
