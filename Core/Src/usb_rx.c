/*
 * usb_rx.c
 *
 *  Created on: May 22, 2025
 *      Author: cyfin
 */

#include "tusb.h"
#include "cobs.h"
#include "registry.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "tile_data.h"
#include "serial_terminal.h"

#define MESSAGE_MAX_LEN 8  // Max COBS-decoded message length (adjust as needed)

uint8_t message_buf[MESSAGE_MAX_LEN];
uint8_t message_len = 0;

static uint8_t received_first_delimiter = 0;

uint8_t cobs_decoded_message[MESSAGE_MAX_LEN];

void USB_ProcessMessage(uint8_t *buffer, uint16_t bufsize) {
	// step 1: COBS decode the message
	cobs_decode_result cobs_result = cobs_decode(cobs_decoded_message, MESSAGE_MAX_LEN, buffer, bufsize);
	if (cobs_result.status != COBS_DECODE_OK || cobs_result.out_len < 3) {
		return;
	}
	uint8_t data_size = cobs_result.out_len - 2;
	// if the first byte is 0x00, the message is trying to write to a master register
	if (cobs_decoded_message[0] == 0x00) {
		uint8_t reg = cobs_decoded_message[1];
		Registry_RegConfig *reg_config = (Registry_RegConfig*) &registry_table[reg];
		if (reg < 0x80) {
			if (reg_config->enabled && reg_config->access == Registry_READWRITE && data_size == reg_config->size) {
				memcpy((void*) reg_config->addr, &cobs_decoded_message[2], reg_config->size);
			}
		} else {
			// handle command endpoints
			if (reg & 0x80) {
				if (reg == 0x80 && data_size == 4) {
					// this command means set coil setpoint for tile <byte 1> and coil index <byte 2> with value <bytes 3-4>
					uint8_t tile_id = cobs_decoded_message[2];
					uint8_t coil_index = cobs_decoded_message[3];
					uint16_t setpoint;
					memcpy(&setpoint, &cobs_decoded_message[4], sizeof(setpoint));
					if (tile_id < MAX_TILES && coil_index < 9) {
						coil_setpoints[tile_id][coil_index] = setpoint;
					}
				}
				if (reg == 0x82 && data_size == 1) {
					TileData_IdentifyTile(cobs_decoded_message[2]);
				}
			}
		}
	}
}

void tud_vendor_rx_cb(uint8_t itf, uint8_t const *buffer, uint16_t bufsize) {
	for (uint16_t i = 0; i < bufsize; i++) {
		uint8_t byte = buffer[i];

		if (byte == 0x00) {
			received_first_delimiter = 1;
			if (message_len > 2) {
				USB_ProcessMessage(message_buf, message_len);
			}
			message_len = 0;
		} else {
			if (message_len < MESSAGE_MAX_LEN && received_first_delimiter) {
				message_buf[message_len++] = byte;
			} else {
				message_len = 0;
			}
		}
	}
	tud_vendor_read_flush();
}

char cdc_message[MAX_CDC_MSG_LEN];
unsigned int cdc_message_len = 0;
void tud_cdc_rx_cb(uint8_t itf) {
    uint32_t bytes_available;
	while (bytes_available = tud_cdc_available()) {
		unsigned int count = tud_cdc_read((uint8_t *)cdc_message, sizeof(cdc_message) - 1);
		if (count > 0) {
			for (unsigned int i = 0; i < count; i++) {
				char c = cdc_message[i];
				if ((c == '\r' || c == '\n') && cdc_message_len > 0) {
					cdc_message[cdc_message_len++] = '\0'; // Null-terminate the string
					SerialTerminal_HandleCommand(cdc_message);
					cdc_message_len = 0; // Reset for next command
				} else if (c == '\0' || cdc_message_len >= sizeof(cdc_message) - 1) {
					continue;
				} else {
					cdc_message[cdc_message_len++] = c;
				}
			}
		}
	}
}
