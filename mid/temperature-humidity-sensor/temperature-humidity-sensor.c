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
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <app/framework/include/af.h>
#include <em_i2c.h>
#include "source/mid/temperature-humidity-sensor/temperature-humidity-sensor.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define I2C_GPIO							 gpioPortB
#define I2C0_PIN_SDA			    		 	 1
#define I2C0_PIN_SCL			    			 0
#define SCAN_TEMP_AND_HUMI						10000

/* I2C device address for si7020 */
#define SI7020_ADDR            0x40

/* Device ID value for si7020 */
#define SI7020_DEVICE_ID       0x14

/* si7020 Read Temperature Command */
#define SI7020_READ_TEMP       0xE0

/* si7020 Read RH Command */
#define SI7020_READ_RH         0xE5

/* si7020 ID */
#define SI7020_READ_ID_ONE       0xFC
#define SI7020_READ_ID_TWO       0xC9
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
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


/******************************************************************************/
/**
 * @func    initI2C
 * @brief   Initialize I2C communicate sensor
 * @param   None
 * @retval  None
 */
void initI2C(void)
{
  CMU_ClockEnable(cmuClock_I2C0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Use default settings
  I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

  // Using PB0 (SDA) and PB1 (SCL)
  GPIO_PinModeSet(I2C_GPIO, I2C0_PIN_SDA, gpioModeWiredAndPullUpFilter, 1);
  GPIO_PinModeSet(I2C_GPIO, I2C0_PIN_SCL, gpioModeWiredAndPullUpFilter, 1);

  // Route I2C pins to GPIO
  GPIO->I2CROUTE[0].SDAROUTE = (GPIO->I2CROUTE[0].SDAROUTE & ~_GPIO_I2C_SDAROUTE_MASK)
                        | (I2C_GPIO << _GPIO_I2C_SDAROUTE_PORT_SHIFT
                        | (I2C0_PIN_SDA << _GPIO_I2C_SDAROUTE_PIN_SHIFT));
  GPIO->I2CROUTE[0].SCLROUTE = (GPIO->I2CROUTE[0].SCLROUTE & ~_GPIO_I2C_SCLROUTE_MASK)
                        | (I2C_GPIO << _GPIO_I2C_SCLROUTE_PORT_SHIFT
                        | (I2C0_PIN_SCL << _GPIO_I2C_SCLROUTE_PIN_SHIFT));
  GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;

  // Initialize the I2C
  I2C_Init(I2C0, &i2cInit);

  // Enable automatic STOP on NACK
  I2C0->CTRL = I2C_CTRL_AUTOSN;

}

/**
 * @func    si7020_Init
 * @brief   Initialize si7020
 * @param   None
 * @retval  None
 */
void Si7020_Init (void){
	I2C_TransferSeq_TypeDef    seq;
	I2C_TransferReturn_TypeDef ret;
	u8_t                    i2c_read_data[3];	// detect ID: si7020_DEVICE_ID  and checksum byte
	u8_t                    i2c_write_data[2];	// command Electronic ID 2nd Byte



	seq.addr  = SI7020_ADDR << 1 ;  			//(Address si7020: 0x40 << 1 for bit R/W)
	seq.flags = I2C_FLAG_WRITE_READ;
	/* Select command to issue */
	i2c_write_data[0] = SI7020_READ_ID_ONE;
	i2c_write_data[1] = SI7020_READ_ID_TWO;
	seq.buf[0].data   = i2c_write_data;
	seq.buf[0].len    = 2;

	/* Select location/length of data to be read */
	seq.buf[1].data = i2c_read_data;
	seq.buf[1].len  = 3;

	ret = I2C_TransferInit(I2C0, &seq);
	// Sending data
	while (ret == i2cTransferInProgress)
	{
		ret = I2C_Transfer(I2C0);
	}
	emberAfCorePrintln (" Detect ret: %d",ret);
	if ((ret != i2cTransferDone) | (i2c_read_data[0] != SI7020_DEVICE_ID)) {

		emberAfCorePrintln (" Detect si7020 failure!--Device Id:%d ",i2c_read_data[0]);

	}
	emberAfCorePrintln (" Detect si7020 success! ");
}


/**
 * @func    si7020_Measure
 * @brief   Measure
 * @param   u32_t *pBuffer
 *          u8_t byCommand
 *          u8_t byLengthData
 * @retval  TRUE or FALSE
 */
bool_t Si7020_Measure (u32_t *pBuffer, u8_t byCommand, u8_t byLengthData){
	I2C_TransferSeq_TypeDef    seq;
	I2C_TransferReturn_TypeDef retVal;
	u8_t  byI2C_read_data[byLengthData];  		//i2c_read_data[0]: MSB		i2c_read_data[1]: LSB
	u8_t  byI2C_write_data[1];

	seq.addr  = SI7020_ADDR << 1; //(Address si7020: 0x40 << 1 for bit R/W)
	seq.flags = I2C_FLAG_WRITE_READ;
	/* Select command to issue */
	byI2C_write_data[0] = byCommand;			// command
	seq.buf[0].data   = byI2C_write_data;
	seq.buf[0].len    = 1;
	/* Select location/length of data to be read */
	seq.buf[1].data = byI2C_read_data;
	seq.buf[1].len  = 2;

	retVal = I2C_TransferInit(I2C0, &seq);
	// Sending data
	while (retVal == i2cTransferInProgress)
	{
		retVal = I2C_Transfer(I2C0);
	}

	*pBuffer = ((u32_t) byI2C_read_data[0] << 8) + (byI2C_read_data[1]);   // // RH_CODE (MSB << 8) + LSB
	return true;
}

/**
 * @func    si7020_MeasureTempAndHumi
 * @brief   Measure Humi
 * @param   None
 * @retval  byHumiValue
 */
u32_t Si7020_MeasureHumi(void){
	u32_t byHumiValue;
	bool_t retVal = Si7020_Measure (&byHumiValue, SI7020_READ_RH, 2);
	if (retVal)
	    byHumiValue = ( (((byHumiValue) * 12500) >> 16) - 600 )/100; // Humi = ((Humi * 2500)/(0xFFu +1) - 600)/100
	return byHumiValue;
}

/**
 * @func    si7020_MeasureTempAndHumi
 * @brief   Measure Temp
 * @param
 * @retval  byTempValue
 */
u32_t Si7020_MeasureTemp (void){
	u32_t byTempValue;
	bool_t retVal = Si7020_Measure (&byTempValue, SI7020_READ_TEMP, 2);
	if (retVal)
	    byTempValue = ( (((byTempValue) * 17572) >> 16) - 4685 )/100; // Temp = ((Temp * 17572)/(0xFFu +1) - 4685)/100
	return byTempValue;
}
