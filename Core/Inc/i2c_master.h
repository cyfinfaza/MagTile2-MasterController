/*
 * i2c_master.h
 *
 *  Created on: Apr 12, 2025
 *      Author: cyfin
 */

#ifndef INC_I2C_MASTER_H_
#define INC_I2C_MASTER_H_

#include <stdint.h>
#include "stm32h5xx_hal.h"
#include "mt2_types.h"
#include "tile_data.h"

typedef struct {
	uint8_t device_present: 1;
	uint8_t bus_error: 1;
	uint8_t checksum_error: 1;
	uint8_t bogus: 1;
} I2C_ReqStatus;

void I2C_Init(I2C_HandleTypeDef *hi2c_selection);
I2C_ReqStatus I2C_ReadTileReg(uint8_t addr, uint8_t reg, void *data, uint16_t size);
uint8_t I2C_ReadTile_StatusOnly(uint8_t addr);
void I2C_ReadTile_Full(uint8_t addr);
void I2C_ReadAllTiles_Full(void);
void I2C_ReadAllTiles_StatusOnly(void);
void I2C_IterativeReadAllTiles(void);

#endif /* INC_I2C_MASTER_H_ */
