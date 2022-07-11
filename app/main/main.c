
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
 * File name: main.c
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
#include <source/app/main/main.h>
#include <source/app/network/network.h>
#include <source/app/receive/receive.h>
#include <source/app/send/send.h>
#include <source/mid/Button/Button.h>
#include <source/mid/kalman-filter/kalman-filter.h>
#include <source/mid/led/led.h>
#include <source/mid/light-sensor/light-sensor.h>
#include <source/mid/temperature-humidity-sensor/temperature-humidity-sensor.h>
#include "app/framework/include/af.h"
#include "protocol/zigbee/stack/include/binding-table.h"
#include "math.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define PERIOD_SCAN_SENSORLIGHT									10000 	//	ms
#define PERIOD_SCAN_SENSORTEMHUMI								10000	//	ms
#define ENDPOINT_ONOFF_LED1										1		
#define ENDPOINT_ONOFF_LED2										2
#define ENDPOINT_LIGHT											3										
#define ENDPOINT_TEMP											4

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
bool boNetworkReady = false;
SystemState_e systemState = POWER_ON_STATE;
uint32_t byValueLuxFirst = 0;
uint32_t byValueLuxSecond = 0;

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
void Main_ButtonPressCallbackHandler(uint8_t byButton, ButtonState_e buttonPressHandler);
void Main_ButtonHoldCallbackHandler(uint8_t byButton, ButtonState_e buttonHoldingHandler);
void mainNetworkEventHandler(uint8_t byNetworkResult);
void emberIncomingManyToOneRouteRequestHandler(EmberNodeId addressNetwork,
                                            EmberEUI64 exPanID,
                                            uint8_t byCost);
/******************************************************************************/
/*                            EVENTS CONTROL                            	  */
/******************************************************************************/
EmberEventControl ReadValueLightSensorControl;
EmberEventControl ReadValueTempHumiControl;
EmberEventControl mainStateEventControl;
EmberEventControl MTORRsEventControl;
EmberEventControl FindNetworkControl;
/** @brief Main Init
 *
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup.
 * Any code that you would normally put into the top of the application's
 * main() routine should be put into this function.
        Note: No callback
 * in the Application Framework is associated with resource cleanup. If you
 * are implementing your application on a Unix host where resource cleanup is
 * a consideration, we expect that you will use the standard Posix system
 * calls, including the use of atexit() and handlers for signals such as
 * SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If you use the signal()
 * function to register your signal handler, please mind the returned value
 * which may be an Application Framework function. If the return value is
 * non-null, please make sure that you call the returned function from your
 * handler to avoid negating the resource cleanup of the Application Framework
 * itself.
 *
 */
void emberAfMainInitCallback(void)
{
	emberAfCorePrintln("Main Init");
	ledInit();
	LDRInit();
	initI2C();
	Si7020_Init();
	byButtonInit(Main_ButtonHoldCallbackHandler, Main_ButtonPressCallbackHandler);
	Network_Init(mainNetworkEventHandler);
	emberEventControlSetActive(mainStateEventControl);
	KalmanFilterInit(2, 2, 0.001); // Initialize Kalman filter
	emberEventControlSetDelayMS(ReadValueLightSensorControl, 1000);
	emberEventControlSetDelayMS(ReadValueTempHumiControl,1000);
}
/******************************************************************************/
/**
 * @func    Main_byButtonPressCallbackHandler
 * @brief   Handler State Press Button
 * @param   uint8_t byButton
 * 			ButtonState_e buttonPressHandler
 * @retval  None
 */
void Main_ButtonPressCallbackHandler(uint8_t byButton, ButtonState_e buttonPressHandler)
{
	switch(buttonPressHandler)
		{
		case PRESS_1:
			if(byButton == BUTTON_1)
			{
				emberAfCorePrintln("SW1: 1 time");
				turnOnLed(LED1,ledBlue);
				SEND_OnOffStateReport(ENDPOINT_ONOFF_LED1, LED_ON);
				SEND_BindingInitToTarget(SOURCE_ENDPOINT_PRIMARY,
										DESTINATTION_ENDPOINT,
										true,
										HC_NETWORK_ADDRESS);
			}
			else
			{
				emberAfCorePrintln("SW2: 1 time");
				turnOnLed(LED2,ledBlue);
				SEND_OnOffStateReport(ENDPOINT_ONOFF_LED2, LED_ON);

			}
		break;
		case PRESS_2:
			if(byButton == BUTTON_1)
			{
				emberAfCorePrintln("SW1: 2 times");
				SEND_OnOffStateReport(ENDPOINT_ONOFF_LED1, LED_OFF);
				turnOffRBGLed(LED1);
				SEND_BindingInitToTarget(SOURCE_ENDPOINT_PRIMARY,
										DESTINATTION_ENDPOINT,
										false,
										HC_NETWORK_ADDRESS);
			}
			else
			{
				emberAfCorePrintln("SW2: 2 time");
				turnOffRBGLed(LED2);
				SEND_OnOffStateReport(ENDPOINT_ONOFF_LED2, LED_OFF);
			}
			break;
		case PRESS_3:
			if(byButton == BUTTON_1)
			{
				emberAfCorePrintln("SW1: 3 time");
				emberAfPluginFindAndBindTargetStart(1);
				toggleLed(LED2,ledyellow,3,200,200);


			}
			else
			{
				emberAfCorePrintln("SW2: 3 time");
				emberAfPluginFindAndBindInitiatorStart(1);
				toggleLed(LED2,ledRGB,3,200,200);
			}
			break;
		case PRESS_4:
			if(byButton == BUTTON_1)
			{
				emberAfCorePrintln("SW1: 4 time");
			}
			else
			{
				emberAfCorePrintln("SW2: 4 time");
			}
			break;
		case PRESS_5:
			if(byButton == BUTTON_1)
			{
				emberAfCorePrintln("SW1: 5 time");

			}
			else
			{
				emberAfCorePrintln("SW2: 5 time");
				toggleLed(LED1,ledRed, 2, 200, 200);
				systemState = REBOOT_STATE;
				emberEventControlSetDelayMS(mainStateEventControl,3000);
			}
			break;
		default:
			break;
		}
}
/**
 * @func    Main_ButtonHoldCallbackHandler
 * @brief   Handler State Holding Button
 * @param   uint8_t byButton
 * 			ButtonState_e buttonHoldingHandler
 * @retval  None
 */
void Main_ButtonHoldCallbackHandler(uint8_t byButton, ButtonState_e buttonHoldingHandler)
{
	switch(buttonHoldingHandler)
	{
	case HOLD_1s:
		emberAfCorePrintln("SW1: 1 s");
		break;
	case HOLD_2s:
		emberAfCorePrintln("SW1: 2 s");
		break;
	case HOLD_3s:
		emberAfCorePrintln("SW1: 3 s");
		break;
	case HOLD_4s:
		emberAfCorePrintln("SW1: 4 s");
		break;
	case HOLD_5s:
		emberAfCorePrintln("SW1: 5 s");
		break;
	default:
		break;
	}
}
/**
 * @func    mainStateEventHandler
 * @brief   Handler Event State System
 * @param   None
 * @retval  None
 */
void mainStateEventHandler(void)
{
	emberEventControlSetInactive(mainStateEventControl);
	EmberNetworkStatus nwkStatusCurrent;
	switch (systemState) {
		case POWER_ON_STATE:
			nwkStatusCurrent = emberAfNetworkState();
			if(nwkStatusCurrent == EMBER_NO_NETWORK)
			{
				toggleLed(LED1,ledRed,3,200,200);
				emberEventControlSetInactive(FindNetworkControl);
				emberEventControlSetActive(FindNetworkControl);
			}
			systemState = IDLE_STATE;
			break;
		case REPORT_STATE:
			systemState = IDLE_STATE;
			SEND_ReportInfoHc();
			break;
		case IDLE_STATE:
				emberAfCorePrintln("IDLE_STATE");
			break;
		case REBOOT_STATE:
			systemState = IDLE_STATE;
			EmberNetworkStatus networkStatus = emberAfNetworkState();
			if (networkStatus != EMBER_NO_NETWORK) {
				SendZigDevRequest();
				emberClearBindingTable();
				emberLeaveNetwork();
			} else {
				halReboot();
			}
			break;
		default:
			break;
	}
}
/**
 * @func    mainNetworkEventHandler
 * @brief   Handler Event State Network
 * @param   None
 * @retval  None
 */
void mainNetworkEventHandler(uint8_t byNetworkResult)
{
	emberAfCorePrintln("Network Event Handle");
	switch (byNetworkResult) {
		case NETWORK_HAS_PARENT:
			emberAfCorePrintln("Network has parent");
			toggleLed(LED1,ledPink,3,300,300);
			boNetworkReady = true;
			systemState = REPORT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		case NETWORK_JOIN_FAIL:
			systemState = IDLE_STATE;
			toggleLed(LED1,ledBlue,3,300,300);
			emberAfCorePrintln("Network Join Fail");
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		case NETWORK_JOIN_SUCCESS:
			emberEventControlSetInactive(FindNetworkControl);
			emberAfCorePrintln("Network Join Success");
			toggleLed(LED1,ledPink,3,300,300);
			boNetworkReady =true;
			systemState = REPORT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		case NETWORK_LOST_PARENT:
			emberAfCorePrintln("Network lost parent");
			toggleLed(LED1,ledPink,3,300,300);
			systemState = IDLE_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		case NETWORK_OUT_NETWORK:
			if(boNetworkReady)
			{
				toggleLed(LED1,ledRed,3,300,300);
				systemState = REBOOT_STATE;
				emberEventControlSetDelayMS(mainStateEventControl, 3000);
			}
			break;
		default:
			break;
	}
}
/**
 * @func    FindNetworkHandler
 * @brief   Handler Event Find Network
 * @param   None
 * @retval  None
 */
void FindNetworkHandler(void)
{
	emberEventControlSetInactive(FindNetworkControl);
	NETWORK_FindAndJoin();
	emberEventControlSetDelayMS(FindNetworkControl, 10000);

}
/**
 * @func    ReadValueLightSensorControl
 * @brief   Read Value Lux from Ldr Sensor
 * @param   None
 * @retval  None
 */
void ReadValueLightSensorControl(void)
{
	emberEventControlSetInactive(ReadValueLightSensorControl);
	byValueLuxFirst = LightSensor_AdcPollingRead();
	if(abs(byValueLuxSecond - byValueLuxFirst) > 30)
		{
		byValueLuxSecond = byValueLuxFirst;
		SEND_LDRStateReport(ENDPOINT_LIGHT,byValueLuxSecond);
		emberAfCorePrintln("Light:   %d lux         ",byValueLuxSecond);
			if(byValueLuxSecond > 500)
			{
				turnOnLed(LED2,ledGreen);
			}
			else
			{
				turnOffRBGLed(LED2);
			}
		}
	emberEventControlSetDelayMS(ReadValueLightSensorControl,PERIOD_SCAN_SENSORLIGHT);
}
/**
 * @func    ReadValueLightSensorControl
 * @brief   Read Value Temp-Humi from Si7020 Sensor
 * @param   None
 * @retval  None
 */
void ReadValueTempHumiHandler(void)
{
	emberEventControlSetInactive(ReadValueTempHumiControl);
	uint32_t Humi = Si7020_MeasureHumi();
	uint32_t Temp = Si7020_MeasureTemp();
	SEND_TempStateReport(ENDPOINT_TEMP,Temp);
	emberAfCorePrintln("Humi:    %d RH       Temp:     %d oC        ", Humi,Temp);
	emberEventControlSetDelayMS(ReadValueTempHumiControl,PERIOD_SCAN_SENSORTEMHUMI);
}
/**
 * @func    emberIncomingManyToOneRouteRequestHandler
 * @brief  	Send a Many to One Message to All Devices
 * @param   None
 * @retval  None
 */
void emberIncomingManyToOneRouteRequestHandler(EmberNodeId addressNetwork,
                                            EmberEUI64 exPanID,
                                            uint8_t byCost)
{
	// handle for MTORRs
	emberAfCorePrintln("Received MTORRs");
	emberEventControlSetInactive(MTORRsEventControl);
	emberEventControlSetDelayMS(MTORRsEventControl,2* ((uint8_t)halCommonGetRandom()));			// 1-2 minutes
}
/**
 * @func    MTORRsEventHandler
 * @brief  	Many to One Event Handler
 * @param   None
 * @retval  None
 */
void MTORRsEventHandler(void)
{
	emberEventControlSetInactive(MTORRsEventControl);
	uint8_t byDataPtr;
	uint8_t byDataLenght = 1;
	EmberAfStatus status = emberAfReadServerAttribute(RGB1_ENDPOINT,
											ZCL_ON_OFF_CLUSTER_ID,
											ZCL_ON_OFF_ATTRIBUTE_ID,
											&byDataPtr,
											byDataLenght);
	if(status == EMBER_ZCL_STATUS_SUCCESS){
		SEND_OnOffStateReport(RGB1_ENDPOINT,byDataPtr);
	}

	EmberAfStatus status1 = emberAfReadServerAttribute(RGB2_ENDPOINT,
												ZCL_ON_OFF_CLUSTER_ID,
												ZCL_ON_OFF_ATTRIBUTE_ID,
												&byDataPtr,
												byDataLenght);
		if(status1 == EMBER_ZCL_STATUS_SUCCESS){
			SEND_OnOffStateReport(RGB2_ENDPOINT,byDataPtr);
		}
}
