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
EmberEventControl joinNetworkEventControl;
/******************************************************************************/
/*                              PRIVATE DATA                                  */
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
u32_t bytimeFindAndJoin = 0;
networkEventHandler networkEventHandle = NULL;
static void joinNetworkEventHandler(void);
static void NETWORK_StopFindAndJoin(void);
/******************************************************************************/
/*
 * @function 			: Network_Init
 *
 * @brief				: Handle event network.
 *
 * @parameter			: networkEventHandler
 *
 * @return value		: None
 */
void networkInit(networkEventHandler networkResult)
{
	networkEventHandle = networkResult;
}

/*
 * @function 			: NETWORK_FindAndJoin
 *
 * @brief				: Find network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
void NETWORK_FindAndJoin(void)
{
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberEventControlSetDelayMS(joinNetworkEventControl, 2000);
	}
}


/*
 * @function 			: NETWORK_StopFindAndJoin
 *
 * @brief				: Stop find network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
static void NETWORK_StopFindAndJoin(void)
{
	emberAfPluginNetworkSteeringStop();
}


/*
 * @function 			: joinNetworkEventHandler
 *
 * @brief				: Handle Event Join network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
static void joinNetworkEventHandler(void)
{
	emberEventControlSetInactive(joinNetworkEventControl);

	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberAfPluginNetworkSteeringStart();
		bytimeFindAndJoin++;
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
		if(bytimeFindAndJoin>0)// vao mang thanh cong
		{
			NETWORK_StopFindAndJoin();
			if(networkEventHandle != NULL)
			{
				networkEventHandle(NETWORK_JOIN_SUCCESS);
				emberAfCorePrintln("NETWORK_JOIN_SUCCESS");
			}
		}else
		{
			if(networkEventHandle != NULL)
			{
				networkEventHandle(NETWORK_HAS_PARENT);
				emberAfCorePrintln("NETWORK_HAS_PARENT");
			}
		}

	}
	else
	{
		EmberNetworkStatus nwkStatusCurrent = emberAfNetworkState();
		if(nwkStatusCurrent == EMBER_NO_NETWORK)
		{
			if(networkEventHandle != NULL)
			{
				networkEventHandle(NETWORK_OUT_NETWORK);
				emberAfCorePrintln("NETWORK_OUT_NETWORK");
			}
		}
		else if(nwkStatusCurrent == EMBER_JOINED_NETWORK_NO_PARENT){
			emberAfCorePrintln("NETWORK_LOST_PARENT");
			networkEventHandle(NETWORK_LOST_PARENT);
		}
	}
	if(networkStatus == EMBER_JOIN_FAILED)
	{
		emberAfCorePrintln("NETWORK_JOIN_FAIL");
		networkEventHandle(NETWORK_JOIN_FAIL);
	}
	return FALSE;
}


