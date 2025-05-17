/*
 * tile_data.c
 *
 *  Created on: May 17, 2025
 *      Author: cyfin
 */

#include "tile_data.h"
#include "mt2_types.h"
#include "tile_reg_xmacro.h"

MT2_Slave_Data tile_data[MAX_TILES];

int TileData_Write(uint8_t addr, uint8_t reg, void *data, uint16_t size) {
	if (addr >= MAX_TILES) {
		return -1; // Invalid address
	}

#define X(_reg, _name, _access, _type) \
	if (reg == _reg && size == sizeof(_type)) { \
		tile_data[addr]._name = *(const _type *)data; \
		return 0; \
	}

	TILE_REG_XMACRO

#undef X

	return -2; // Invalid register or size mismatch
}
