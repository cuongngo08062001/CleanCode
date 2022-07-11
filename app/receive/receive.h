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
 * File name: receive.h
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
#ifndef APP_RECEIVE_RECEIVE_H_
#define APP_RECEIVE_RECEIVE_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stdbool.h>							1
/* Function prototypes -----------------------------------------------*/
uint8_t checkBindingTable(uint8_t byLocalEndpoint);
bool emberAfPreCommandReceivedCallback(EmberAfClusterCommand* clusterCmd);
bool RECEIVE_HandleLevelControlCluster(EmberAfClusterCommand* clusterCmd);
bool RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* clusterCmd);
#endif /* APP_RECEIVE_RECEIVE_H_ */
