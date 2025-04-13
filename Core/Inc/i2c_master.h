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

#define MAX_TILES 25

typedef union {
	uint8_t byte;
	struct {
		uint8_t alive: 1;
		uint8_t arm_ready: 1;
		uint8_t arm_active: 1;
		uint8_t coils_nonzero: 1;
	} flags;
} MT2_Slave_Status;

typedef union {
	uint8_t byte;
	struct {
		uint8_t temp_fault: 1;
		uint8_t current_spike_fault: 1;
		uint8_t vsense_fault: 1;
		uint8_t invalid_value_fault: 1;
	} flags;
} MT2_Slave_Faults;

typedef union {
	uint8_t byte;
	struct {
		uint8_t global_arm: 1;
		uint8_t global_fault_clear: 1;
	} flags;
} MT2_Master_Status;

typedef union {
	uint8_t byte;
	struct {
		uint8_t identify: 1;
		uint8_t local_fault_clear: 1;
	} flags;
} MT2_Slave_Settings;

typedef struct {
	uint8_t device_present: 1;
	uint8_t bus_error: 1;
	uint8_t checksum_error: 1;
	uint8_t bogus: 1;
} I2C_ReqStatus;

typedef struct {
	MT2_Slave_Status slave_status;
	MT2_Slave_Faults slave_faults;
	MT2_Master_Status master_status;
	MT2_Slave_Settings slave_settings;

	float v_sense_5;
	float v_sense_12;
	float v_sense_hv;
	uint16_t mcu_temp;

	uint8_t adj_west_addr;
	uint8_t adj_north_addr;
	uint8_t adj_east_addr;
	uint8_t adj_south_addr;

	uint16_t coil_1_setpoint;
	uint16_t coil_2_setpoint;
	uint16_t coil_3_setpoint;
	uint16_t coil_4_setpoint;
	uint16_t coil_5_setpoint;
	uint16_t coil_6_setpoint;
	uint16_t coil_7_setpoint;
	uint16_t coil_8_setpoint;
	uint16_t coil_9_setpoint;
	uint16_t coil_1_current_reading;
	uint16_t coil_2_current_reading;
	uint16_t coil_3_current_reading;
	uint16_t coil_4_current_reading;
	uint16_t coil_5_current_reading;
	uint16_t coil_6_current_reading;
	uint16_t coil_7_current_reading;
	uint16_t coil_8_current_reading;
	uint16_t coil_9_current_reading;
	int16_t coil_1_temp;
	int16_t coil_2_temp;
	int16_t coil_3_temp;
	int16_t coil_4_temp;
	int16_t coil_5_temp;
	int16_t coil_6_temp;
	int16_t coil_7_temp;
	int16_t coil_8_temp;
	int16_t coil_9_temp;
} MT2_Slave_Data;

extern MT2_Slave_Data mt2_slave_data[MAX_TILES];

void I2C_Init(I2C_HandleTypeDef *hi2c_selection);
I2C_ReqStatus I2C_ReadTileReg(uint8_t addr, uint8_t reg, void *data, uint16_t size);
uint8_t I2C_ReadTile_Alive(uint8_t addr);
void I2C_ReadTile_Full(uint8_t addr);
void I2C_ReadAllTiles(void);

#endif /* INC_I2C_MASTER_H_ */
