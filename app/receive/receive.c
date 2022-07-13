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
 * File name: receive.c
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
#include <cluster-id.h>
#include <app/framework/include/af.h>
#include "source/mid/led/led.h"
#include "source/app/send/send.h"
#include "source/app/receive/receive.h"
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static u8_t checkBindingTable(u8_t byLocalEndpoint);
bool emberAfPreCommandReceivedCallback(EmberAfClusterCommand* clusterCmd);
static bool_t RECEIVE_HandleLevelControlCluster(EmberAfClusterCommand* clusterCmd);
static bool_t RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* clusterCmd);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                               */
/******************************************************************************/

/**
 * @func    emberAfPreCommandReceivedCallback
 * @brief   Process Command Received
 * @param   EmberAfClusterCommand clusterCmd (Command Cluster)
 * @retval  TRUE or FALSE
 */
bool emberAfPreCommandReceivedCallback(EmberAfClusterCommand* clusterCmd)
{
	if(clusterCmd->clusterSpecific)
	{
		switch(clusterCmd->apsFrame->clusterId)
		{
			case ZCL_ON_OFF_CLUSTER_ID:
				RECEIVE_HandleOnOffCluster(clusterCmd);
				break;
			case ZCL_LEVEL_CONTROL_CLUSTER_ID:
				RECEIVE_HandleLevelControlCluster(clusterCmd);
				break;
			default:
				break;
		}
	}
	return FALSE;
}


/**
 * @func    emberAfPreMessageReceivedCallback
 * @brief   Process Pre message received
 * @param   EmberAfIncomingMessage incommingMessage (message in comming)
 * @retval  TRUE or FALSE
 */
bool emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incommingMessage)
{
	if(incommingMessage->apsFrame->clusterId == ACTIVE_ENDPOINTS_RESPONSE)
	{
		return TRUE;
	}
 return FALSE;
}


/**
 * @func    RECEIVE_HandleLevelControlCluster
 * @brief   Handler Level led
 * @param   EmberAfClusterCommand clusterCmd
 * @retval  TRUE or FALSE
 */
static bool_t RECEIVE_HandleLevelControlCluster(EmberAfClusterCommand* clusterCmd)
{
	u8_t commandID = clusterCmd->commandId;
	u8_t endPoint  = clusterCmd->apsFrame -> destinationEndpoint;
	u8_t payloadOffset = clusterCmd->payloadStartIndex;		// Gan offset = startindex
	u8_t level;
	uint16_t transitionTime;
	emberAfCorePrintln("ClusterID: 0x%2X", clusterCmd->apsFrame->clusterId);
/******************************************LEVEL CONTROL LED***************************************************************************/
		switch(commandID)
			{

				case ZCL_MOVE_TO_LEVEL_COMMAND_ID:
					if(clusterCmd->bufLen < payloadOffset + 1u)
					{
						return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
					}
					level = emberAfGetInt8u(clusterCmd->buffer, payloadOffset, clusterCmd->bufLen);		// Lay 1 byte gia tri cua level control
					transitionTime = emberAfGetInt16u(clusterCmd->buffer, payloadOffset+1, clusterCmd->bufLen);
					emberAfCorePrintln(" RECEIVE_HandleLevelControlCluster LEVEL = %d, time: 0x%2X\n", level, transitionTime);

					if(endPoint == 1)
					{
						if(level >=80)
						{
							emberAfCorePrintln("LED GREEN");
							turnOnLed(LED_ONE, LED_GREEN);
						}else if(level>=40)
						{
							emberAfCorePrintln("LED RED");
							turnOnLed(LED_ONE, LED_RED);
						}else if(level>0)
						{
							emberAfCorePrintln("LED BLUE");
							turnOnLed(LED_ONE, LED_BLUE);
						}else
						{
							emberAfCorePrintln("turn 0ff");
							turnOffRBGLed(LED_ONE);
						}

					}
					break;
				default:
					break;
				}
		return FALSE;
}


/**
 * @func    RECEIVE_HandleOnOffCluster
 * @brief   Handler On/Off command
 * @param   EmberAfClusterCommand clusterCmd (Command Cluster)
 * @retval  static bool_t
 */
static bool_t RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* clusterCmd)
{
	u8_t commandID = clusterCmd->commandId;
	u8_t localEndpoint = clusterCmd ->apsFrame -> destinationEndpoint;
	u8_t remoteEndpoint = clusterCmd->apsFrame -> sourceEndpoint;
	uint16_t IgnoreNodeID = clusterCmd->source;
/************************************ON-OFF LED******************************************************************************************/
	emberAfCorePrintln("RECEIVE_HandleOnOffCluster SourceEndpoint = %d, RemoteEndpoint = %d, commandID = %d, nodeID %2X\n",remoteEndpoint,localEndpoint,commandID,IgnoreNodeID);
	switch(commandID)
	{
	case ZCL_OFF_COMMAND_ID:
		emberAfCorePrintln("Turn off LED");

		switch (clusterCmd->type) {
			case EMBER_INCOMING_UNICAST:
				if(localEndpoint == 1)
				{
				turnOffRBGLed(LED_ONE);
				SEND_OnOffStateReport(localEndpoint, LED_OFF);
				emberAfCorePrintln("check: %d",checkBindingTable(localEndpoint));
					if(checkBindingTable(localEndpoint) >= 1)
					{

						SEND_BindingInitToTarget(remoteEndpoint, localEndpoint, FALSE, IgnoreNodeID);
					}
				}
				if(localEndpoint == 2)
				{
					turnOffRBGLed(LED_TWO);
					SEND_OnOffStateReport(localEndpoint, LED_OFF);
				}
				break;
			case EMBER_INCOMING_MULTICAST:
				emberAfCorePrintln("Multicast");
				turnOnLed(LED_ONE, LED_OFF);
				turnOnLed(LED_TWO, LED_OFF);
				break;
			default:
				break;
		}

		break;
	case ZCL_ON_COMMAND_ID:

		emberAfCorePrintln("Turn on LED");
		switch (clusterCmd->type) {
			case EMBER_INCOMING_UNICAST:
				if(localEndpoint == 1)
				{
					turnOnLed(LED_ONE, LED_BLUE);
					SEND_OnOffStateReport(localEndpoint, LED_ON);
					if(checkBindingTable(localEndpoint) >= 1)
					{
						SEND_BindingInitToTarget(remoteEndpoint, localEndpoint, TRUE, IgnoreNodeID);
					}
				}
				if(localEndpoint == 2)
				{
					turnOnLed(LED_TWO, LED_BLUE);
					SEND_OnOffStateReport(localEndpoint, LED_ON);
				}
				break;
			case EMBER_INCOMING_MULTICAST:
				emberAfCorePrintln("Multicast");
				turnOnLed(LED_TWO, LED_GREEN);
				break;
			default:
				break;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

/*
 * @function 			: checkBindingTable
 *
 * @brief				: API support to check information on binding table.
 *
 * @parameter			: localEndpoint
 *
 * @return value		: Index in Binding Table
 */


static u8_t checkBindingTable(u8_t byLocalEndpoint)
{
	u8_t byIndex = 0;
	for(u8_t i = 0; i< EMBER_BINDING_TABLE_SIZE; i++)
	{
		EmberBindingTableEntry binding;
		if(emberGetBindingRemoteNodeId(i) != EMBER_SLEEPY_BROADCAST_ADDRESS){
			emberGetBinding(i, &binding);
			if(binding.local == byLocalEndpoint && (binding.type == EMBER_UNICAST_BINDING))
			{
			    byIndex++;
			}
		}
	}
	return byIndex;
}




