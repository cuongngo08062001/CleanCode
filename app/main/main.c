
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
#include <app/framework/include/af.h>
#include <protocol/zigbee/stack/include/binding-table.h>
#include <math.h>
#include "source/app/network/network.h"
#include "source/app/receive/receive.h"
#include "source/app/send/send.h"
#include "source/mid/button/button.h"
#include "source/mid/kalman-filter/kalman-filter.h"
#include "source/mid/led/led.h"
#include "source/mid/light-sensor/light-sensor.h"
#include "source/mid/temperature-humidity-sensor/temperature-humidity-sensor.h"
#include "source/app/main/main.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define PERIOD_SCAN_SENSORLIGHT									10000 	//	ms
#define PERIOD_SCAN_SENSORTEMHUMI								10000	//	ms
#define DELAY_MS												1000
enum{
	ENDPOINT_ONOFF_LED_ONE	= 1,
	ENDPOINT_ONOFF_LED_TWO,
	ENDPOINT_LIGHT,
	ENDPOINT_TEMP
};

enum{
	RGB1_ENDPOINT	= 1,
	RGB2_ENDPOINT,
	LIGH_ENDPOINT,
	TEMP_ENDPOINT
};
typedef enum{
	POWER_ON_STATE,
	REPORT_STATE,
	IDLE_STATE,
	REBOOT_STATE
}SystemState;
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
bool_t g_boNetworkReady = FALSE;
SystemState g_systemState = POWER_ON_STATE;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void mainButtonPressCallbackHandler(u8_t byButtonId, ButtonState buttonPressState);
static void mainButtonHoldCallbackHandler(u8_t byButtonId, ButtonState buttonHoldingState);
static void mainNetworkEventHandler(u8_t byNetworkResult);
void emberIncomingManyToOneRouteRequestHandler(EmberNodeId addressNetwork,
                                            EmberEUI64 exPanID,
											u8_t byCost);
EmberAfStatus emberAfPluginFindAndBindTargetStart(u8_t byEndpoint);
EmberStatus emberAfPluginFindAndBindInitiatorStart(u8_t byEndpoint);
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
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
/******************************************************************************/
/*                            EVENTS CONTROL                            	  */
/******************************************************************************/
EmberEventControl ReadValueLightSensorControl;
EmberEventControl ReadValueTempHumiControl;
EmberEventControl MainStateEventControl;
EmberEventControl MTORRsEventControl;
EmberEventControl FindNetworkControl;

/******************************************************************************/

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
	lightSensor_Init();
	initI2C();
	Si7020_Init();
	buttonInit(mainButtonHoldCallbackHandler, mainButtonPressCallbackHandler);
	networkInit(mainNetworkEventHandler);
	emberEventControlSetActive(MainStateEventControl);
	KalmanFilterInit(2, 2, 0.001); // Initialize Kalman filter
	emberEventControlSetDelayMS(ReadValueLightSensorControl, DELAY_MS);
	emberEventControlSetDelayMS(ReadValueTempHumiControl,DELAY_MS);
}
/******************************************************************************/
/**
 * @func    Main_byButtonPressCallbackHandler
 * @brief   Handler State Press Button
 * @param   u8_t byButton
 * 			ButtonState_e buttonPressState
 * @retval  None
 */
static void mainButtonPressCallbackHandler(u8_t byButtonId, ButtonState buttonPressState)
{
	switch(buttonPressState)
		{
		case PRESS_1:
			if(byButtonId == BUTTON_ONE)
			{
				emberAfCorePrintln("SW1: 1 time");
				turnOnLed(LED_ONE,LED_BLUE);
				SEND_OnOffStateReport(ENDPOINT_ONOFF_LED_ONE, LED_ON);
				SEND_BindingInitToTarget(SOURCE_ENDPOINT_PRIMARY,
										 DESTINATTION_ENDPOINT,
										 TRUE,
										 HC_NETWORK_ADDRESS);
			}
			else
			{
				emberAfCorePrintln("SW2: 1 time");
				turnOnLed(LED_TWO,LED_BLUE);

				SEND_OnOffStateReport(ENDPOINT_ONOFF_LED_TWO, LED_ON);

			}
		break;
		case PRESS_2:
			if(byButtonId == BUTTON_ONE)
			{
				emberAfCorePrintln("SW1: 2 times");
				SEND_OnOffStateReport(ENDPOINT_ONOFF_LED_ONE, LED_OFF);
				turnOffRBGLed(LED_ONE);
				SEND_BindingInitToTarget(SOURCE_ENDPOINT_PRIMARY,
										DESTINATTION_ENDPOINT,
										FALSE,
										HC_NETWORK_ADDRESS);
			}
			else
			{
				emberAfCorePrintln("SW2: 2 time");
				turnOffRBGLed(LED_TWO);
				SEND_OnOffStateReport(ENDPOINT_ONOFF_LED_TWO, LED_OFF);
			}
			break;
		case PRESS_3:
			if(byButtonId == BUTTON_ONE)
			{
				emberAfCorePrintln("SW1: 3 time");
				emberAfPluginFindAndBindTargetStart(1);
				toggleLed(LED_TWO,LED_YELLOW,3,200,200);


			}
			else
			{
				emberAfCorePrintln("SW2: 3 time");
				emberAfPluginFindAndBindInitiatorStart(1);
				toggleLed(LED_TWO,LED_RGB,3,200,200);
			}
			break;
		case PRESS_4:
			if(byButtonId == BUTTON_ONE)
			{
				emberAfCorePrintln("SW1: 4 time");
			}
			else
			{
				emberAfCorePrintln("SW2: 4 time");
			}
			break;
		case PRESS_5:
			if(byButtonId == BUTTON_ONE)
			{
				emberAfCorePrintln("SW1: 5 time");

			}
			else
			{
				emberAfCorePrintln("SW2: 5 time");
				toggleLed(LED_ONE,LED_RED, 2, 200, 200);
				g_systemState = REBOOT_STATE;
				emberEventControlSetDelayMS(MainStateEventControl,3000);
			}
			break;
		default:
			break;
		}
}
/**
 * @func    Main_ButtonHoldCallbackHandler
 * @brief   Handler State Holding Button
 * @param   u8_t byButton
 * 			ButtonState_e buttonHoldingHandler
 * @retval  None
 */
static void mainButtonHoldCallbackHandler(u8_t byButtonId, ButtonState buttonHoldingState)
{
	switch(buttonHoldingState)
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
	emberEventControlSetInactive(MainStateEventControl);
	EmberNetworkStatus nwkStatusCurrent;
	switch (g_systemState) {
		case POWER_ON_STATE:
			nwkStatusCurrent = emberAfNetworkState();
			if(nwkStatusCurrent == EMBER_NO_NETWORK)
			{
				toggleLed(LED_ONE,LED_RED,3,200,200);
				emberEventControlSetInactive(FindNetworkControl);
				emberEventControlSetActive(FindNetworkControl);
			}
			g_systemState = IDLE_STATE;
			break;
		case REPORT_STATE:
			g_systemState = IDLE_STATE;
			SEND_ReportInfoHc();
			break;
		case IDLE_STATE:
				emberAfCorePrintln("IDLE_STATE");
			break;
		case REBOOT_STATE:
			g_systemState = IDLE_STATE;
			EmberNetworkStatus networkStatus = emberAfNetworkState();
			if (networkStatus != EMBER_NO_NETWORK) {
			    //SendZigDevRequest();
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
static void mainNetworkEventHandler(u8_t byNetworkResult)
{
	emberAfCorePrintln("Network Event Handle");
	switch (byNetworkResult) {
		case NETWORK_HAS_PARENT:
			emberAfCorePrintln("Network has parent");
			toggleLed(LED_ONE,LED_PINK,3,300,300);
			g_boNetworkReady = TRUE;
			g_systemState = REPORT_STATE;
			emberEventControlSetDelayMS(MainStateEventControl, DELAY_MS);
			break;
		case NETWORK_JOIN_FAIL:
			g_systemState = IDLE_STATE;
			toggleLed(LED_ONE,LED_BLUE,3,300,300);
			emberAfCorePrintln("Network Join Fail");
			emberEventControlSetDelayMS(MainStateEventControl, DELAY_MS);
			break;
		case NETWORK_JOIN_SUCCESS:
			emberEventControlSetInactive(FindNetworkControl);
			emberAfCorePrintln("Network Join Success");
			toggleLed(LED_ONE,LED_PINK,3,300,300);
			g_boNetworkReady =TRUE;
			g_systemState = REPORT_STATE;
			emberEventControlSetDelayMS(MainStateEventControl, DELAY_MS);
			break;
		case NETWORK_LOST_PARENT:
			emberAfCorePrintln("Network lost parent");
			toggleLed(LED_ONE,LED_PINK,3,300,300);
			g_systemState = IDLE_STATE;
			emberEventControlSetDelayMS(MainStateEventControl, DELAY_MS);
			break;
		case NETWORK_OUT_NETWORK:
			if(g_boNetworkReady)
			{
				toggleLed(LED_ONE,LED_PINK,3,300,300);
				g_systemState = REBOOT_STATE;
				emberEventControlSetDelayMS(MainStateEventControl, 3000);
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
	emberEventControlSetDelayMS(FindNetworkControl, DELAY_MS*10);

}
/**
 * @func    ReadValueLightSensorControl
 * @brief   Read Value Lux from Ldr Sensor
 * @param   None
 * @retval  None
 */
void ReadValueLightSensorHandler(void)
{
	u32_t byValueLuxSecond = 0;
//	emberEventControlSetInactive(readValueLightSensorControl);
	u32_t byValueLuxFirst = readAdcPolling_LightSensorHandler();
	if(abs(byValueLuxSecond - byValueLuxFirst) > 30)
		{
		byValueLuxSecond = byValueLuxFirst;
		SEND_LDRStateReport(ENDPOINT_LIGHT,byValueLuxSecond);
		emberAfCorePrintln("Light:   %d lux         ",byValueLuxSecond);
			if(byValueLuxSecond > 500)
			{
				turnOnLed(LED_TWO,LED_GREEN);
			}
			else
			{
				turnOffRBGLed(LED_TWO);
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
	u32_t byHumi = Si7020_MeasureHumi();
	u32_t byTemp = Si7020_MeasureTemp();
	SEND_TempStateReport(ENDPOINT_TEMP,byTemp);
	emberAfCorePrintln("Humi:    %d RH       Temp:     %d oC        ", byHumi,byTemp);
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
                                            u8_t byCost)
{
	// handle for MTORRs
	emberAfCorePrintln("Received MTORRs");
	emberEventControlSetInactive(MTORRsEventControl);
	emberEventControlSetDelayMS(MTORRsEventControl,2* ((u8_t)halCommonGetRandom()));			// 1-2 minutes
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
	u8_t byDataPtr;
	u8_t byDataLenght = 1;
	EmberAfStatus statusOne = emberAfReadServerAttribute(RGB1_ENDPOINT,
											ZCL_ON_OFF_CLUSTER_ID,
											ZCL_ON_OFF_ATTRIBUTE_ID,
											&byDataPtr,
											byDataLenght);
	if(statusOne == EMBER_ZCL_STATUS_SUCCESS){
		SEND_OnOffStateReport(RGB1_ENDPOINT,byDataPtr);
	}

	EmberAfStatus statusTwo = emberAfReadServerAttribute(RGB2_ENDPOINT,
												ZCL_ON_OFF_CLUSTER_ID,
												ZCL_ON_OFF_ATTRIBUTE_ID,
												&byDataPtr,
												byDataLenght);
		if(statusTwo == EMBER_ZCL_STATUS_SUCCESS){
			SEND_OnOffStateReport(RGB2_ENDPOINT,byDataPtr);
		}
}
