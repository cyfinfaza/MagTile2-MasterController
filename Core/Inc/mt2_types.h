/*
 * mt2_types.h
 *
 *  Created on: May 17, 2025
 *      Author: cyfin
 */

#ifndef INC_MT2_TYPES_H_
#define INC_MT2_TYPES_H_

#include <stdint.h>
#include "tile_reg_xmacro.h"

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
	#define X(addr, name, access, type) type name;
		TILE_REG_XMACRO
	#undef X
} MT2_Slave_Data;

#endif /* INC_MT2_TYPES_H_ */
