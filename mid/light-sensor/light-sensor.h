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
 * File name: light-sensor.h
 *
 * Description:
 *
 * Author: CuongNV
 *
 * Last Changed By:  $Author: cuongnv $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $Jun 7, 2022
 ******************************************************************************/

#ifndef SOURCE_MID_LDR_LDR_H_
#define SOURCE_MID_LDR_LDR_H_
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
// Set CLK_ADC to 10MHz
#define CLK_SRC_ADC_FREQ          1000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ              10000 // CLK_ADC - 10MHz max in normal mode
/* Function prototypes -----------------------------------------------*/
void LDRInit(void);
uint32_t readAdcPolling_LightSensor(void);
#endif /* SOURCE_MID_LDR_LDR_H_ */
