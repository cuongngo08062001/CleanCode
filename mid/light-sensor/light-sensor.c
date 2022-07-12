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
 * File name: light-sensor.c
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
#include <math.h>
#include "source/mid/kalman-filter/kalman-filter.h"
#include <em_iadc.h>
#include "source/mid/light-sensor/light-sensor.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define IADC_INPUT_0_PORT_PIN     iadcPosInputPortCPin5;
#define IADC_INPUT_1_PORT_PIN     iadcNegInputGnd;

#define IADC_INPUT_0_BUS          CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_CDBUSALLOC_CDEVEN0_ADC0
#define CLK_SRC_ADC_FREQ          1000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ              10000 // CLK_ADC - 10MHz max in normal mode
/******************************************************************************/
/*                              CONTROL EVENTS                    	 		  */
/******************************************************************************/
EmberEventControl readValueSensorLightControl;
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
 * @func    LDRInit
 * @brief   LDR initialize
 * @param   None
 * @retval  None
 */
void lightSensor_Init(void)
{
	// Declare init structs
	  IADC_Init_t init = IADC_INIT_DEFAULT;
	  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
	  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
	  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;

	  // Enable IADC clock
	  CMU_ClockEnable(cmuClock_IADC0, true);
	  // enable pin

	  // Reset IADC to reset configuration in case it has been modified
	  IADC_reset(IADC0);

	  // Configure IADC clock source for use while in EM2
	  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

	  // Modify init structs and initialize
	  init.warmup = iadcWarmupKeepWarm;

	  // Set the HFSCLK prescale value here
	  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

	  // Configuration 0 is used by both scan and single conversions by default
	  // Use unbuffered AVDD as reference
	  initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;

	  // Divides CLK_SRC_ADC to set the CLK_ADC frequency for desired sample rate
	  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
	                                                                    CLK_ADC_FREQ,
	                                                                    0,
	                                                                    iadcCfgModeNormal,
	                                                                    init.srcClkPrescale);

	  // Set oversampling rate to 32x
	  // resolution formula res = 11 + log2(oversampling * digital averaging)
	  // in this case res = 11 + log2(32*1) = 16
	  initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed32x;


	  // Single initialization
	  initSingle.dataValidLevel = _IADC_SINGLEFIFOCFG_DVL_VALID1;

	  // Set conversions to run continuously
	//  initSingle.triggerAction = iadcTriggerActionContinuous;

	  // Set alignment to right justified with 16 bits for data field
	//  initSingle.alignment = iadcAlignRight16;

	  // Configure Input sources for single ended conversion
	  initSingleInput.posInput = iadcPosInputPortCPin5;
	  initSingleInput.negInput = iadcNegInputGnd;

	  // Initialize IADC
	  // Note oversampling and digital averaging will affect the offset correction
	  // This is taken care of in the IADC_init() function in the emlib
	  IADC_init(IADC0, &init, &initAllConfigs);

	  // Initialize Scan
	  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

	  // Allocate the analog bus for ADC0 inputs
	  GPIO->IADC_INPUT_0_BUS |= GPIO_CDBUSALLOC_CDODD0_ADC0;  //IADC_INPUT_BUSALLOC
}

/**
 * @func   readAdcPolling_LightSensorHandler
 * @brief  Read value from ADC
 * @param  None
 * @retval None
 */
i32_t readAdcPolling_LightSensorHandler(void)
{
	volatile i32_t byRegistor;
	volatile i32_t byLux = 0;
	i32_t byValueADC=0;
	i32_t byKalman_Light=0;
	emberEventControlSetInactive(readValueSensorLightControl);
	IADC_Result_t iadcResult;
	// Start IADC conversion
	IADC_command(IADC0, iadcCmdStartSingle);

	// Wait for conversion to be complete
	while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK
				| _IADC_STATUS_SINGLEFIFODV_MASK)) != IADC_STATUS_SINGLEFIFODV); //while combined status bits 8 & 6 don't equal 1 and 0 respectively

	// Get ADC result
	iadcResult = IADC_pullSingleFifoResult(IADC0);
	byValueADC = iadcResult.data;
	byKalman_Light = Kalman_sensor(byValueADC);
	// Calculate input voltage:
	//  For differential inputs, the resultant range is from -Vref to +Vref, i.e.,
	//  for Vref = AVDD = 3.30V, 12 bits represents 6.60V full scale IADC range.
	byRegistor= 10000*(3300 - byKalman_Light)/(byKalman_Light);    // byRegistor  = 10K*ADC / (4095 -ADC)
	byLux = abs(316*pow(10,5)*pow(byRegistor,-1.4));
	return byLux;
}

