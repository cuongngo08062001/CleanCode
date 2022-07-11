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
 * File name: temparature_humidity_sensor.h
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
#ifndef SOURCE_MID_TEMPHUMI_SI7020_H_
#define SOURCE_MID_TEMPHUMI_SI7020_H_
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stdbool.h>
/******************************************************************************/
/*                             DEFINITIONS        			                  */
/******************************************************************************/

#define I2C_GPIO							 gpioPortB
#define I2C0_PIN_SDA			    		 	 1
#define I2C0_PIN_SCL			    			 0
#define SCAN_TEMP_AND_HUMI						10000

/* I2C device address for Si7020 */
#define SI7020_ADDR            0x40

/* Device ID value for Si7020 */
#define SI7020_DEVICE_ID       0x14

/* Si7020 Read Temperature Command */
#define SI7020_READ_TEMP       0xE0

/* Si7020 Read RH Command */
#define SI7020_READ_RH         0xE5

/* SI7020 ID */
#define SI7020_READ_ID_1       0xFC
#define SI7020_READ_ID_2       0xC9

/* Function prototypes -----------------------------------------------*/
void initI2C(void);
void Si7020_Init (void);
bool Si7020_Measure (uint32_t *pBuffer, uint8_t byCommand, uint8_t byLengthData);
uint32_t Si7020_MeasureHumi (void);
uint32_t Si7020_MeasureTemp (void);
/******************************************************************************/
#endif /* SOURCE_MID_TEMPHUMI_SI7020_H_ */

