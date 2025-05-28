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
		uint8_t shutdown_from_fault: 1;
	} flags;
} MT2_Slave_Status;

typedef union {
	uint8_t byte;
	struct {
		uint8_t temp_fault: 1;
		uint8_t current_spike_fault: 1;
		uint8_t vsense_fault: 1; // TODO
		uint8_t invalid_value_fault: 1;
		uint8_t communication_fault: 1;
	} flags;
} MT2_Slave_Faults;

typedef union {
	uint8_t byte;
	struct {
		uint8_t global_arm: 1;
		uint8_t global_fault_clear: 1;
	} flags;
} MT2_Global_State;

typedef union {
	uint8_t byte;
	struct {
		uint8_t identify: 1;
		uint8_t local_fault_clear: 1;
	} flags;
} MT2_Slave_Settings;

typedef union {
	uint8_t byte;
	struct {
		uint8_t hv_relay_on: 1;
		uint8_t precharge_ssr_on: 1;
		uint8_t shdn_12_on: 1;
		uint8_t fault_12: 1;
		uint8_t hv_shutdown_from_fault: 1;
		uint8_t hv_ready: 1;
	} flags;
} MT2_Master_PowerSwitchStatus;

typedef union {
	uint8_t byte;
	struct {
		uint8_t vendor_active: 1;
		uint8_t cdc_active: 1;
	} flags;
} MT2_Master_UsbInterfaceStatus;

typedef union {
	uint16_t byte;
	struct {
		uint8_t ov_5v: 1;
		uint8_t uv_5v: 1;
		uint8_t oc_5v: 1;
		uint8_t ov_12v: 1;
		uint8_t uv_12v: 1;
		uint8_t oc_12v: 1;
		uint8_t ov_hv: 1;
		uint8_t uv_hv: 1;
		uint8_t oc_hv: 1;
		uint8_t efuse_12v_fault: 1;
		uint8_t master_overtemp: 1;
		uint8_t precharge_fault: 1;
		uint8_t slave_fault: 1;
		uint8_t communication_fault: 1;
	} flags;
} MT2_Master_PowerSystemFaults;

typedef struct {
	#define X(addr, name, access, type) type name;
		TILE_REG_XMACRO
	#undef X
} MT2_Slave_Data;

#endif /* INC_MT2_TYPES_H_ */
