/*
 * reporter.c
 *
 *  Created on: Apr 13, 2025
 *      Author: cyfin
 */

#include "reporter.h"
#include <stdint.h>
#include <string.h>
#include "tusb.h"
#include "tile_data.h"
#include "cobs.h"
#include "tile_reg_xmacro.h"
#include "registry.h"

#define TILE_REGISTRY_SIZE 128
#define MAX_MSG_SIZE 16

uint8_t iter_tile_addr = 1;
uint8_t iter_tile_reg = 0;
uint8_t iter_master_reg = 0;

// function to cobs encode a message with addr, reg, and data
static int Reporter_EncodeMessage(uint8_t *encoded, uint8_t addr, uint8_t reg, void *data, uint16_t data_len) {
	uint8_t buffer[MAX_MSG_SIZE];

	// prepare the message
	buffer[0] = addr;
	buffer[1] = reg;
	memcpy(&buffer[2], data, data_len);

	// cobs encode the message
	cobs_encode_result cobs_result = cobs_encode(encoded, MAX_MSG_SIZE, buffer, data_len + 2);

	// add a zero at the end of the message
	encoded[cobs_result.out_len] = 0x00;

	if (cobs_result.status != COBS_ENCODE_OK) {
		return -1; // encoding error
	} else {
		return cobs_result.out_len + 1;
	}
}

// not idempotent
void Reporter_IterativeReportAllTiles() {

	if (!tud_vendor_mounted() || tud_vendor_write_available() < MAX_MSG_SIZE) {
		return;
	}

	uint8_t buffer[MAX_MSG_SIZE];
	uint8_t data[MAX_MSG_SIZE];
	uint16_t data_len = 0;
	for (int i = 0; i < TILE_REGISTRY_SIZE * MAX_TILES; i++) {
		int result = TileData_Read(iter_tile_addr, iter_tile_reg, data, &data_len);
		if (result == 0) {
			// encode the message
			int encoded_size = Reporter_EncodeMessage(buffer, iter_tile_addr, iter_tile_reg, data, data_len);
			if (encoded_size > 0) {
				tud_vendor_write(buffer, encoded_size);
//				tud_vendor_write_flush();
			}
		}
		if (++iter_tile_reg >= TILE_REGISTRY_SIZE) {
			iter_tile_reg = 0;
			if (++iter_tile_addr > MAX_TILES) {
				iter_tile_addr = 1; // reset to first tile
			}
		}
		if (result == 0) {
			return;
		}
	}
}

void Reporter_IterativeReportMaster() {

	if (!tud_vendor_mounted() || tud_vendor_write_available() < MAX_MSG_SIZE) {
		return;
	}

	uint8_t buffer[MAX_MSG_SIZE];
	for (int i = 0; i < Registry_GetTotal(); i++) {
		char reg_enabled = registry_table[iter_master_reg].enabled;
		if (reg_enabled) {
			int encoded_size = Reporter_EncodeMessage(buffer, 0x00, iter_master_reg, (void *)registry_table[iter_master_reg].addr, registry_table[iter_master_reg].size);
			if (encoded_size > 0) {
				tud_vendor_write(buffer, encoded_size);
			}
		}
		if (++iter_master_reg >= Registry_GetTotal()) {
			iter_master_reg = 0;
		}
		if (reg_enabled) {
			return;
		}
	}
}


