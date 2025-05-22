/*
 * tile_data.c
 *
 *  Created on: May 17, 2025
 *      Author: cyfin
 */

#include "tile_data.h"
#include "mt2_types.h"
#include "tile_reg_xmacro.h"
#include "stm32h5xx_hal.h"
#include <string.h>

MT2_Slave_Data tile_data[MAX_TILES];

uint32_t tile_last_seen[MAX_TILES];

int TileData_Write(uint8_t addr, uint8_t reg, void *data, uint16_t size) {
	if (addr >= MAX_TILES) {
		return -1; // Invalid address
	}

#define X(_reg, _name, _access, _type) \
	if (reg == _reg && size == sizeof(_type)) { \
		memcpy(&tile_data[addr]._name, data, sizeof(_type)); \
		return 0; \
	}

	TILE_REG_XMACRO

#undef X

	return -2; // Invalid register or size mismatch
}

int TileData_Read(uint8_t addr, uint8_t reg, void *data, uint16_t* size) {
	if (addr >= MAX_TILES) {
		return -1; // Invalid address
	}

#define X(_reg, _name, _access, _type) \
	if (reg == _reg) { \
		memcpy(data, &tile_data[addr]._name, sizeof(_type)); \
		if (size) { \
			*size = sizeof(_type); \
		} \
		return 0; \
	}
	TILE_REG_XMACRO
#undef X
	return -2; // Invalid register or size mismatch
}

void TileData_JustHeardFrom(uint8_t addr) {
	if (addr < MAX_TILES) {
		tile_last_seen[addr] = HAL_GetTick();
	}
}

void TileData_MarkInactiveTiles() {
	for (uint8_t i = 0; i < MAX_TILES; i++) {
		if (tile_last_seen[i] + ALIVE_TIMEOUT < HAL_GetTick()) {
			tile_data[i].slave_status.flags.alive = 0;
		}
	}
}
