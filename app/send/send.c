
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
 * File name: led.h
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
#include <source/app/receive/receive.h>
#include <source/app/send/send.h>
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "app/framework/include/af.h"
#include "zigbee-framework/zigbee-device-common.h"

/******************************************************************************/
/**
 * @func    SEND_SendCommandUnicast
 * @brief   Send uinicast command
 * @param   source, destination, address
 * @retval  None
 */
static void SEND_SendCommandUnicast(uint8_t bySourceEndpoint,
							 uint8_t byDestinationEndpoint,
							 uint8_t byIndexOrDestination)
{
	emberAfSetCommandEndpoints(bySourceEndpoint, bySourceEndpoint);
	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, byIndexOrDestination);
}


/**
 * @func    SEND_FillBufferGlobalCommand
 * @brief   Send fill buffer global command
 * @param   clusterID, attributeID, byGlobalCommand, pValue, byLength, byDataType
 * @retval  None
 */
static void SEND_FillBufferGlobalCommand(EmberAfClusterId clusterID,
								  EmberAfAttributeId attributeID,
								  uint8_t byGlobalCommand,
								  uint8_t* pValue,
								  uint8_t byLength,
								  uint8_t byDataType)
{
	uint8_t data[MAX_DATA_COMMAND_SIZE];
	data[0] = (uint8_t)(attributeID & 0x00FF);
	data[1] = (uint8_t)((attributeID & 0xFF00)>>8);
	data[2] = EMBER_SUCCESS;
	data[3] = (uint8_t)byDataType;
	memcpy(&data[4], pValue, pValue);

	(void) emberAfFillExternalBuffer((ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
									  clusterID,
									  byGlobalCommand,
									  "b",
									  data,
									  byLength + 4);
}


/**
 * @func    SEND_ReportInfoHc
 * @brief   Send Report to HC
 * @param   None
 * @retval  None
 */
void SEND_ReportInfoHc(void)
{
	uint8_t modelID[13] = {12, 'S', 'W', '2','_','L','M','1','_','T','M','P','1'};
	uint8_t manufactureID[5] = {4, 'L', 'u', 'm', 'i'};
	uint8_t version = 1;

	if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
		return;
	}
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 modelID,
								 13,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);

	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MANUFACTURER_NAME_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 manufactureID,
								 5,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_APPLICATION_VERSION_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 &version,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);
}


/**
 * @func    SEND_OnOffStateReport
 * @brief   Send On/Off State
 * @param   Endpoint, byValue
 * @retval  None
 */
void SEND_OnOffStateReport(uint8_t Endpoint, uint8_t byValue){
	SEND_FillBufferGlobalCommand(ZCL_ON_OFF_CLUSTER_ID,
						   ZCL_ON_OFF_ATTRIBUTE_ID,
						   ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
						   (uint8_t*) &byValue,
						   1,
						   ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(Endpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(Endpoint,
								ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								(uint8_t*) &byValue,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}


/**
 * @func    SEND_PIRStateReport
 * @brief   Send PIR byValue to App
 * @param   Endpoint, byValue
 * @retval  None
 */
void SEND_PIRStateReport(uint8_t byEndpoint, uint8_t byValue){
	SEND_FillBufferGlobalCommand(ZCL_IAS_ZONE_CLUSTER_ID,
								 ZCL_ZONE_STATUS_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint8_t*) &byValue,
								 1,
						   	   	 ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(byEndpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(byEndpoint,
								ZCL_IAS_ZONE_CLUSTER_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								(uint8_t*) &byValue,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}

/**
 * @func    SEND_LDRStateReport
 * @brief   Send lux value to app
 * @param   source, destination, address
 * @retval  None
 */
void SEND_LDRStateReport(uint8_t byEndpoint, uint32_t byValue){
	SEND_FillBufferGlobalCommand(ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,
								 ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint32_t*) &byValue,
								 sizeof(byValue),
								 ZCL_INT32U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(byEndpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(byEndpoint,
								ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,
								ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID,
								(uint32_t*) &byValue,
								ZCL_INT32U_ATTRIBUTE_TYPE);
}

/**
 * @func    SEND_TempStateReport
 * @brief   Send Temp value
 * @param   Endpoint, value
 * @retval  None
 */
void SEND_TempStateReport(uint8_t byEndpoint, uint32_t byValue){
	SEND_FillBufferGlobalCommand(ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
								 ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint32_t*) &byValue,
								 sizeof(byValue),
								 ZCL_INT32U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(byEndpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(byEndpoint,
								ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
								ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID,
								(uint32_t*) &byValue,
								ZCL_INT32U_ATTRIBUTE_TYPE);
}


/**
 * @func    SEND_BindingInitToTarget
 * @brief   Send Binding command
 * @param   byRemoteEndpoint, byLocalEndpoint, boValue, byNodeID
 * @retval  None
 */
SEND_BindingInitToTarget(uint8_t byRemoteEndpoint, uint8_t byLocalEndpoint, bool boValue, uint16_t byNodeID){
	EmberStatus status = EMBER_INVALID_BINDING_INDEX;

	for(uint8_t i = 0; i< EMBER_BINDING_TABLE_SIZE ; i++)
		{
			EmberBindingTableEntry binding;
			status = emberGetBinding(i, &binding);
			uint16_t bindingNodeID = emberGetBindingRemoteNodeId(i);

			// check status send
			if(status != EMBER_SUCCESS)
			{
				return;
			}else if((binding.local == byLocalEndpoint) && (binding.remote == byRemoteEndpoint) && (bindingNodeID == byRemoteEndpoint))
			{
				continue;
			}
			else if((bindingNodeID != EMBER_SLEEPY_BROADCAST_ADDRESS) &&
						 (bindingNodeID != EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS) &&
						 (bindingNodeID != EMBER_BROADCAST_ADDRESS))
			{
				if(binding.local == byLocalEndpoint && binding.clusterId == ZCL_ON_OFF_CLUSTER_ID){
					switch (boValue) {
						case true:
							emberAfCorePrintln("SEND ON INIT TO TARGET");
							emberAfFillCommandOnOffClusterOn();
							emberAfSetCommandEndpoints(binding.local, binding.remote);
							emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, bindingNodeID);
							SEND_OnOffStateReport(binding.local, LED_ON);
							break;
						case false:
							emberAfCorePrintln("SEND OFF INIT TO TARGET");
							emberAfFillCommandOnOffClusterOff();
							emberAfSetCommandEndpoints(binding.local, binding.remote);
							emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, bindingNodeID);
							SEND_OnOffStateReport(binding.local, LED_OFF);
							break;
					}
				}
			}
		}
}


