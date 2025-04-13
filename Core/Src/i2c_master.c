/*
 * i2c_master.c
 *
 *  Created on: Apr 12, 2025
 *      Author: cyfin
 */

#include "i2c_master.h"
#include <string.h>

MT2_Slave_Data mt2_slave_data[MAX_TILES];

I2C_HandleTypeDef* hi2c;

void I2C_Init(I2C_HandleTypeDef *hi2c_selection) {
	hi2c = hi2c_selection;
	memset(mt2_slave_data, 0, sizeof(mt2_slave_data));
}

static void I2C_ResetBus(I2C_HandleTypeDef *hi2c) {
	HAL_I2C_DeInit(hi2c);
	HAL_I2C_Init(hi2c);
}

I2C_ReqStatus I2C_ReadTileReg(uint8_t addr, uint8_t reg, void *data, uint16_t size) {
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, addr << 1, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, size, 10000);
	I2C_ReqStatus req_status = {0};
	// check if its a timeout, nack, or other error, then set bits of response accordingly
	req_status.device_present = 1;
	if (status == HAL_OK) {
		// do nothing
	} else if (status == HAL_TIMEOUT) {
		req_status.bus_error = 1;
		I2C_ResetBus(hi2c);
	} else if (status == HAL_ERROR) {
		if (hi2c->ErrorCode & HAL_I2C_ERROR_AF) {
			req_status.device_present = 0;
		} if (hi2c->ErrorCode & ~HAL_I2C_ERROR_AF) {
			req_status.bus_error = 1;
			I2C_ResetBus(hi2c);
		}
	} else {
		req_status.bogus = 1;
		I2C_ResetBus(hi2c);
	}
	return req_status;
}

uint8_t I2C_ReadTile_StatusOnly(uint8_t addr) {
	MT2_Slave_Status stat_temp;
	I2C_ReqStatus status = I2C_ReadTileReg(addr, 0, &stat_temp, 1);
	if (status.device_present) {
		// if the tile is present, store the status and also read fault
		mt2_slave_data[addr].slave_status = stat_temp;
		I2C_ReadTileReg(addr, 0x05, &mt2_slave_data[addr].slave_faults, 1);
	}
	mt2_slave_data[addr].slave_status.flags.alive = status.device_present;
	return status.device_present;
}

void I2C_ReadTile_Full(uint8_t addr) {
	// first check if the tile is alive, then if so read the data
	if (I2C_ReadTile_StatusOnly(addr)) {
		I2C_ReadTileReg(addr, 0x04, &mt2_slave_data[addr].slave_status, 1);
		I2C_ReadTileReg(addr, 0x05, &mt2_slave_data[addr].slave_faults, 1);
		I2C_ReadTileReg(addr, 0x06, &mt2_slave_data[addr].master_status, 1);
		I2C_ReadTileReg(addr, 0x07, &mt2_slave_data[addr].slave_settings, 1);

		I2C_ReadTileReg(addr, 0x08, &mt2_slave_data[addr].v_sense_5, 4);
		I2C_ReadTileReg(addr, 0x09, &mt2_slave_data[addr].v_sense_12, 4);
		I2C_ReadTileReg(addr, 0x0A, &mt2_slave_data[addr].v_sense_hv, 4);
		I2C_ReadTileReg(addr, 0x0B, &mt2_slave_data[addr].mcu_temp, 2);

		I2C_ReadTileReg(addr, 0x0C, &mt2_slave_data[addr].adj_west_addr, 1);
		I2C_ReadTileReg(addr, 0x0D, &mt2_slave_data[addr].adj_north_addr, 1);
		I2C_ReadTileReg(addr, 0x0E, &mt2_slave_data[addr].adj_east_addr, 1);
		I2C_ReadTileReg(addr, 0x0F, &mt2_slave_data[addr].adj_south_addr, 1);

		I2C_ReadTileReg(addr, 0x10, &mt2_slave_data[addr].coil_1_setpoint, 2);
		I2C_ReadTileReg(addr, 0x11, &mt2_slave_data[addr].coil_2_setpoint, 2);
		I2C_ReadTileReg(addr, 0x12, &mt2_slave_data[addr].coil_3_setpoint, 2);
		I2C_ReadTileReg(addr, 0x13, &mt2_slave_data[addr].coil_4_setpoint, 2);
		I2C_ReadTileReg(addr, 0x14, &mt2_slave_data[addr].coil_5_setpoint, 2);
		I2C_ReadTileReg(addr, 0x15, &mt2_slave_data[addr].coil_6_setpoint, 2);
		I2C_ReadTileReg(addr, 0x16, &mt2_slave_data[addr].coil_7_setpoint, 2);
		I2C_ReadTileReg(addr, 0x17, &mt2_slave_data[addr].coil_8_setpoint, 2);
		I2C_ReadTileReg(addr, 0x18, &mt2_slave_data[addr].coil_9_setpoint, 2);
		I2C_ReadTileReg(addr, 0x20, &mt2_slave_data[addr].coil_1_current_reading, 2);
		I2C_ReadTileReg(addr, 0x21, &mt2_slave_data[addr].coil_2_current_reading, 2);
		I2C_ReadTileReg(addr, 0x22, &mt2_slave_data[addr].coil_3_current_reading, 2);
		I2C_ReadTileReg(addr, 0x23, &mt2_slave_data[addr].coil_4_current_reading, 2);
		I2C_ReadTileReg(addr, 0x24, &mt2_slave_data[addr].coil_5_current_reading, 2);
		I2C_ReadTileReg(addr, 0x25, &mt2_slave_data[addr].coil_6_current_reading, 2);
		I2C_ReadTileReg(addr, 0x26, &mt2_slave_data[addr].coil_7_current_reading, 2);
		I2C_ReadTileReg(addr, 0x27, &mt2_slave_data[addr].coil_8_current_reading, 2);
		I2C_ReadTileReg(addr, 0x28, &mt2_slave_data[addr].coil_9_current_reading, 2);
		I2C_ReadTileReg(addr, 0x30, &mt2_slave_data[addr].coil_1_temp, 2);
		I2C_ReadTileReg(addr, 0x31, &mt2_slave_data[addr].coil_2_temp, 2);
		I2C_ReadTileReg(addr, 0x32, &mt2_slave_data[addr].coil_3_temp, 2);
		I2C_ReadTileReg(addr, 0x33, &mt2_slave_data[addr].coil_4_temp, 2);
		I2C_ReadTileReg(addr, 0x34, &mt2_slave_data[addr].coil_5_temp, 2);
		I2C_ReadTileReg(addr, 0x35, &mt2_slave_data[addr].coil_6_temp, 2);
		I2C_ReadTileReg(addr, 0x36, &mt2_slave_data[addr].coil_7_temp, 2);
		I2C_ReadTileReg(addr, 0x37, &mt2_slave_data[addr].coil_8_temp, 2);
		I2C_ReadTileReg(addr, 0x38, &mt2_slave_data[addr].coil_9_temp, 2);
	}
}

void I2C_ReadAllTiles_Full(void) {
	for (uint8_t i = 0; i < MAX_TILES; i++) {
		I2C_ReadTile_Full(i);
	}
}

void I2C_ReadAllTiles_StatusOnly(void) {
	for (uint8_t i = 0; i < MAX_TILES; i++) {
		I2C_ReadTile_StatusOnly(i);
	}
}

uint8_t iterative_read_pos = 0;
void I2C_IterativeReadAllTiles(void) {
	for (uint8_t i = 0; i < MAX_TILES; i++) {
		iterative_read_pos %= MAX_TILES;
		if (mt2_slave_data[iterative_read_pos].slave_status.flags.alive) {
			I2C_ReadTile_Full(iterative_read_pos);
			iterative_read_pos++;
			return;
		}
		iterative_read_pos++;
	}
}

