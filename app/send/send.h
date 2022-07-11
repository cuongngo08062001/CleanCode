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
 * File name: send.h
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
#ifndef APP_SEND_SEND_H_
#define APP_SEND_SEND_H_

/******************************************************************************/
/*                     		DEFINITIONS            				              */
/******************************************************************************/

#define MAX_DATA_COMMAND_SIZE					50
#define SOURCE_ENDPOINT_PRIMARY					1
#define DESTINATTION_ENDPOINT					1
#define HC_NETWORK_ADDRESS						0x0000
#define ZDO_MESSAGE_OVERHEAD 					1
/* Function prototypes -----------------------------------------------*/
void SendZigDevRequest(void);
void SEND_OnOffStateReport(uint8_t byEndpoint, uint8_t byValue);
void SEND_ReportInfoHc(void);
void SendZigDevRequest(void);
void SEND_PIRStateReport(uint8_t byEndpoint, uint8_t byValue);
void SEND_LDRStateReport(uint8_t byEndpoint, uint32_t byValue);
void SEND_TempStateReport(uint8_t byEndpoint, uint32_t byValue);
void SEND_BindingInitToTarget(uint8_t byRemoteEndpoint, uint8_t byLocalEndpoint, bool boValue, uint16_t byNodeID);
#endif /* SOURCE_APP_SEND_SEND_H_ */

