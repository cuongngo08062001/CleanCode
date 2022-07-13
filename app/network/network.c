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
 * File name: network.c
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
#include "source/app/network/network.h"
#include "source/utils/typedefs.h"
/******************************************************************************/
/*                            EVENTS CONTROL                            	  */
/******************************************************************************/
EmberEventControl JoinNetworkEventControl;
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
u32_t g_bytimeFindAndJoin = 0;
networkEventHandler g_networkEvent = NULL;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void networkStopFindAndJoin(void);
/******************************************************************************/
/*
 * @function 			: Network_Init
 *
 * @brief				: Handle event network.
 *
 * @parameter			: g_networkEventHandler
 *
 * @return value		: None
 */
void networkInit(networkEventHandler networkResult)
{
	g_networkEvent = networkResult;
}

/*
 * @function 			: networkFindAndJoin
 *
 * @brief				: Find network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
void networkFindAndJoin(void)
{
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberEventControlSetDelayMS(JoinNetworkEventControl, 2000);
	}
}


/*
 * @function 			: networkStopFindAndJoin
 *
 * @brief				: Stop find network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
static void networkStopFindAndJoin(void)
{
	emberAfPluginNetworkSteeringStop();
}


/*
 * @function 			: joing_networkEventHandler
 *
 * @brief				: Handle Event Join network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
void JoinNetworkEventHandler(void)
{
	emberEventControlSetInactive(JoinNetworkEventControl);

	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberAfPluginNetworkSteeringStart();
		g_bytimeFindAndJoin++;
	}
}


/*
 * @function 			: emberAfStackStatusCallback
 *
 * @brief				: Stack Status
 *
 * @parameter			: EmberStatus
 *
 * @return value		: TRUE or FALSE
 */
bool emberAfStackStatusCallback(EmberStatus networkStatus)
{
	emberAfCorePrintln("emberAfStackStatusCallback\n");

	if(networkStatus == EMBER_NETWORK_UP)
	{
		if(g_bytimeFindAndJoin>0)// vao mang thanh cong
		{
			networkStopFindAndJoin();
			if(g_networkEvent != NULL)
			{
			    g_networkEvent(NETWORK_JOIN_SUCCESS);
				emberAfCorePrintln("NETWORK_JOIN_SUCCESS");
			}
		}else
		{
			if(g_networkEvent != NULL)
			{
			    g_networkEvent(NETWORK_HAS_PARENT);
				emberAfCorePrintln("NETWORK_HAS_PARENT");
			}
		}

	}
	else
	{
		EmberNetworkStatus nwkStatusCurrent = emberAfNetworkState();
		if(nwkStatusCurrent == EMBER_NO_NETWORK)
		{
			if(g_networkEvent != NULL)
			{
			    g_networkEvent(NETWORK_OUT_NETWORK);
				emberAfCorePrintln("NETWORK_OUT_NETWORK");
			}
		}
		else if(nwkStatusCurrent == EMBER_JOINED_NETWORK_NO_PARENT){
			emberAfCorePrintln("NETWORK_LOST_PARENT");
			g_networkEvent(NETWORK_LOST_PARENT);
		}
	}
	if(networkStatus == EMBER_JOIN_FAILED)
	{
		emberAfCorePrintln("NETWORK_JOIN_FAIL");
		g_networkEvent(NETWORK_JOIN_FAIL);
	}
	return FALSE;
}


