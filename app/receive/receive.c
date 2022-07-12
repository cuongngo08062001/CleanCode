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
i8_t checkBindingTable(i8_t byLocalEndpoint);
static static bool_t emberAfPreCommandReceivedCallback(EmberAfClusterCommand* clusterCmd);
static static bool_t RECEIVE_HandleLevelControlCluster(EmberAfClusterCommand* clusterCmd);
static static bool_t RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* clusterCmd);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                               */
/******************************************************************************/

/**
 * @func    emberAfPreCommandReceivedCallback
 * @brief   Process Command Received
 * @param   EmberAfClusterCommand
 * @retval  static bool_tean
 */
static bool_t emberAfPreCommandReceivedCallback(EmberAfClusterCommand* clusterCmd)
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
	return false;
}


/**
 * @func    emberAfPreMessageReceivedCallback
 * @brief   Process Pre message received
 * @param   EmberAfIncomingMessage
 * @retval  None
 */
static bool_t emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incommingMessage)
{
	if(incommingMessage->apsFrame->clusterId == ACTIVE_ENDPOINTS_RESPONSE)
	{
		return true;
	}
 return false;
}


/**
 * @func    RECEIVE_HandleLevelControlCluster
 * @brief   Handler Level led
 * @param   EmberAfClusterCommand
 * @retval  None
 */
static bool_t RECEIVE_HandleLevelControlCluster(EmberAfClusterCommand* clusterCmd)
{
	uint8_t commandID = clusterCmd->commandId;
	uint8_t endPoint  = clusterCmd->apsFrame -> destinationEndpoint;
	uint8_t payloadOffset = clusterCmd->payloadStartIndex;		// Gan offset = startindex
	uint8_t level;
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
		return false;
}


/**
 * @func    RECEIVE_HandleOnOffCluster
 * @brief   Handler On/Off command
 * @param   EmberAfClusterCommand
 * @retval  static bool_t
 */
static bool_t RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* clusterCmd)
{
	uint8_t commandID = clusterCmd->commandId;
	uint8_t localEndpoint = clusterCmd ->apsFrame -> destinationEndpoint;
	uint8_t remoteEndpoint = clusterCmd->apsFrame -> sourceEndpoint;
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

						SEND_BindingInitToTarget(remoteEndpoint, localEndpoint, false, IgnoreNodeID);
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
						SEND_BindingInitToTarget(remoteEndpoint, localEndpoint, true, IgnoreNodeID);
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
	return false;
}

/*
 * @function 			: checkBindingTable
 *
 * @brief				: API support to check information on binding table.
 *
 * @parameter			: localEndpoint
 *
 * @return value		: True or false
 */


uint8_t checkBindingTable(uint8_t localEndpoint)
{
	uint8_t index = 0;
	for(uint8_t i=0; i< EMBER_BINDING_TABLE_SIZE; i++)
	{
		EmberBindingTableEntry binding;
		if(emberGetBindingRemoteNodeId(i) != EMBER_SLEEPY_BROADCAST_ADDRESS){
			emberGetBinding(i, &binding);
			if(binding.local == localEndpoint && (binding.type == EMBER_UNICAST_BINDING))
			{
				index++;
			}
		}
	}
	return index;
}




