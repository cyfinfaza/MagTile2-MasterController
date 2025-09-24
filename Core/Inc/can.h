/*
 * can.h
 *
 *  Created on: May 16, 2025
 *      Author: cyfin
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include "stm32h5xx_hal.h"

void CAN_Init(FDCAN_HandleTypeDef* can_selection);
HAL_StatusTypeDef CAN_SendMessage(uint32_t id, uint8_t* data, uint8_t len);
void CAN_KeepAlive();

#endif /* INC_CAN_H_ */
