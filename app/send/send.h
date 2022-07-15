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
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "source/utils/typedefs.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define MAX_DATA_COMMAND_SIZE					50
#define SOURCE_ENDPOINT_PRIMARY					1
#define DESTINATTION_ENDPOINT					1
#define HC_NETWORK_ADDRESS						0x0000
#define ZDO_MESSAGE_OVERHEAD 					1
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
void SendZigDevRequest(void);
/**
 * @func    SEND_OnOffStateReport
 * @brief   Send On/Off State
 * @param   u8_t Endpoint
 *          u8_t byValue
 * @retval  None
 */
void SEND_OnOffStateReport(u8_t byEndpoint, u8_t byValue);

/**
 * @func    SEND_ReportInfoHc
 * @brief   Send Report to HC
 * @param   None
 * @retval  None
 */
void SEND_ReportInfoHc(void);
/**
 * @func    SEND_PIRStateReport
 * @brief   Send PIR byValue to App
 * @param   u8_t byEndpoint
 *          u8_t byValue
 * @retval  None
 */
void SEND_PIRStateReport(u8_t byEndpoint, u8_t byValue);
/**
 * @func    SEND_LDRStateReport
 * @brief   Send lux value to app
 * @param   u8_t byEndpoint
 *          u32_t byValue
 * @retval  None
 */
void SEND_LDRStateReport(u8_t byEndpoint, u32_t byValue);
/**
 * @func    SEND_TempStateReport
 * @brief   Send Temp value
 * @param   u8_t byEndpoint
 *          u32_t byValue
 * @retval  None
 */
void SEND_TempStateReport(u8_t byEndpoint, u32_t byValue);
/**
 * @func    SEND_BindingInitToTarget
 * @brief   Send Binding command
 * @param   u8_t byRemoteEndpoint,
 *          u8_t byLocalEndpoint,
 *          bool_t boValue,
 *          u16_t byNodeID
 * @retval  None
 */
void SEND_BindingInitToTarget(u8_t byRemoteEndpoint, u8_t byLocalEndpoint, bool_t boValue, i16_t byNodeID);
/******************************************************************************/
#endif /* SOURCE_APP_SEND_SEND_H_ */

