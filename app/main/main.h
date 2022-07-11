
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
 * File name: main.h
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
#ifndef APP_MAIN_MAIN_H_
#define APP_MAIN_MAIN_H_
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define RGB1_ENDPOINT									1
#define RGB2_ENDPOINT									2
#define LIGH_ENDPOINT									3
#define TEMP_ENDPOINT									4
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
typedef enum{
	POWER_ON_STATE,
	REPORT_STATE,
	IDLE_STATE,
	REBOOT_STATE
}SystemState_e;
/* Function prototypes -----------------------------------------------*/
EmberAfStatus emberAfPluginFindAndBindTargetStart(uint8_t byEndpoint);
EmberStatus emberAfPluginFindAndBindInitiatorStart(uint8_t byEndpoint);
#endif /* SOURCE_APP_MAIN_MAIN_H_ */
