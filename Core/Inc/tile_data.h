/*
 * tile_data.h
 *
 *  Created on: May 17, 2025
 *      Author: cyfin
 */

#ifndef INC_TILE_DATA_H_
#define INC_TILE_DATA_H_

#include "mt2_types.h"

#define MAX_TILES 25

extern MT2_Slave_Data tile_data[MAX_TILES];

int TileData_Write(uint8_t addr, uint8_t reg, void *data, uint16_t size);

int TileData_Read(uint8_t addr, uint8_t reg, void *data, uint16_t* size);

#endif /* INC_TILE_DATA_H_ */
