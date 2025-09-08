/*
 * tile_data.h
 *
 *  Created on: May 17, 2025
 *      Author: cyfin
 */

#ifndef INC_TILE_DATA_H_
#define INC_TILE_DATA_H_

#include "mt2_types.h"

#define MAX_TILES 50

#define TILE_MAP_SENTINEL MAX_TILES+1 // must be larger than MAX_TILES

#define ALIVE_TIMEOUT 500 // ms

typedef struct {
	int8_t x;
	int8_t y;
} TileData_Coordinates;

extern MT2_Slave_Data tile_data[MAX_TILES];
extern TileData_Coordinates tile_coordinates[MAX_TILES];
extern uint16_t coil_setpoints[MAX_TILES][9];
extern uint32_t tile_last_seen[MAX_TILES];

int TileData_Write(uint8_t addr, uint8_t reg, void *data, uint16_t size);

int TileData_Read(uint8_t addr, uint8_t reg, void *data, uint16_t* size);

void TileData_JustHeardFrom(uint8_t addr);

void TileData_MarkInactiveTiles();

void TileData_ComputeCoordinates(void);

int TileData_AssignSetpoint(uint8_t x, uint8_t y, uint16_t setpoint);

int TileData_IterativeSendSetpoints(void);

#endif /* INC_TILE_DATA_H_ */
