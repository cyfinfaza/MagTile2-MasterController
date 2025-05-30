/*
 * can.c
 *
 *  Created on: May 16, 2025
 *      Author: cyfin
 */

#include "can.h"
#include <stdint.h>
#include <string.h>
#include "tile_data.h"

FDCAN_HandleTypeDef *hfdcan;

extern uint8_t can_blink;

typedef union {
    uint8_t  u8;
    int8_t   i8;
    uint16_t u16;
    int16_t  i16;
    uint32_t u32;
    int32_t  i32;
    float    f32;
    uint8_t  bytes[4];
} Data4;

Data4 can_rx_test[256];
FDCAN_RxHeaderTypeDef rx_header;

void CAN_Init(FDCAN_HandleTypeDef *can_selection) {
	hfdcan = can_selection;
	HAL_FDCAN_Start(hfdcan);
	HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	FDCAN_FilterTypeDef sFilterConfig;
	sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 0;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = 0x000;        // Accept everything
	sFilterConfig.FilterID2 = 0x000;
	HAL_FDCAN_ConfigFilter(hfdcan, &sFilterConfig);
}

HAL_StatusTypeDef CAN_SendMessage(uint32_t id, uint8_t *data, uint8_t len) {
	FDCAN_TxHeaderTypeDef TxHeader;
	TxHeader.Identifier = id;
	TxHeader.IdType = FDCAN_STANDARD_ID;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader.MessageMarker = 0;
	TxHeader.FDFormat = FDCAN_FD_CAN;           // Enable CAN FD
	TxHeader.BitRateSwitch = FDCAN_BRS_ON; // Enable faster bitrate during data phase
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.DataLength = len;
	return HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeader, data);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *which_fdcan, uint32_t RxFifo0ITs) {
	if (which_fdcan == hfdcan) {
		uint8_t rx_data[64];
		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data)
				!= HAL_OK) {
//			Error_Handler();
		}
		// the 8 LSb of the CAN ID is the address
		uint8_t addr = rx_header.Identifier & 0xFF;
		// the first byte is the address, the rest is data, write to can_rx_test
		uint8_t reg = rx_data[0];
		uint8_t len = rx_header.DataLength - 1;
		if (reg < 256 && len > 0 && len <= 4) {
			memcpy(can_rx_test[reg].bytes, &rx_data[1], len);
		}
		// write to tile_data
		TileData_JustHeardFrom(addr);
		TileData_Write(addr, reg, &can_rx_test[reg], len);
		can_blink = ++can_blink % 2;
	}
}
