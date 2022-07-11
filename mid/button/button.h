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
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define BUTTON_COUNT					2
#define GPIO_DOUT						0
#define BUTTON_DEBOUNCE					5
#define BUTTON_CHECK_HOLD_CYCLES_MS		200
#define BUTTON_CHECK_RELEASE_MS			500

#define HOLD_TIME_MIN					5


#define BUTTON_1_PORT				gpioPortD
#define BUTTON_1_PIN				(4U)

#define BUTTON_2_PORT				gpioPortD
#define BUTTON_2_PIN				(3U)

#define BUTTON_INIT					{{BUTTON_1_PORT,BUTTON_1_PIN},{BUTTON_2_PORT,BUTTON_2_PIN}}



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
}ButtonState_e;

enum
{
	BUTTON_1,
	BUTTON_2
}BUTTON_e;

enum
{
	BUTTON_PRESS,
	BUTTON_RELEASE
}ButtonEvent_e;

typedef struct {
  GPIO_Port_TypeDef		GPIO_Port;
  unsigned int			byPin;
  bool					boState;
  uint8_t				byPressCount;
  uint8_t				byHoldTime;
  bool					boRelease;
  bool					boPress;
  bool                  boIsHolding;
} Buttonx_t;

/* Function prototypes -----------------------------------------------*/
typedef void (*BUTTON_holdingEvent_t)(uint8_t byIndex, ButtonState_e holdEvent );
typedef void (*BUTTON_pressEvent_t)(uint8_t byIndex, ButtonState_e pressEvent);

void buttonInit(BUTTON_holdingEvent_t ButtonHoldingHandler,BUTTON_pressEvent_t ButtonPressHandler);


#endif /* SOURCE_HARD_BUTTON_BUTTON_H_ */
