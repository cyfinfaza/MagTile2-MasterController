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
		Registry_RegConfig *reg = (Registry_RegConfig *)&registry_table[cobs_decoded_message[1]];
		if (reg->enabled && reg->access == Registry_READWRITE && data_size == reg->size) {
			memcpy((void *)reg->addr, &cobs_decoded_message[2], reg->size);
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
}
