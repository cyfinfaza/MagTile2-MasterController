/*
 * tile_reg_xmacro.h
 *
 *  Created on: May 17, 2025
 *      Author: cyfin
 */

#ifndef INC_TILE_REG_XMACRO_H_
#define INC_TILE_REG_XMACRO_H_

#define TILE_REG_XMACRO \
X(0x04, slave_status, READONLY, MT2_Slave_Status) \
X(0x05, slave_faults, READONLY, MT2_Slave_Faults) \
X(0x06, global_state, READWRITE, MT2_Global_State) \
X(0x07, slave_settings, READWRITE, MT2_Slave_Settings) \
\
X(0x08, v_sense_5, READONLY, float) \
X(0x09, v_sense_12, READONLY, float) \
X(0x0A, v_sense_hv, READONLY, float) \
X(0x0B, master_v_sense_hv, READWRITE, float) \
\
X(0x0C, adj_west_addr, READONLY, uint8_t) \
X(0x0D, adj_north_addr, READONLY, uint8_t) \
X(0x0E, adj_east_addr, READONLY, uint8_t) \
X(0x0F, adj_south_addr, READONLY, uint8_t) \
\
X(0x10, coil_1_setpoint, READWRITE, uint16_t) \
X(0x11, coil_2_setpoint, READWRITE, uint16_t) \
X(0x12, coil_3_setpoint, READWRITE, uint16_t) \
X(0x13, coil_4_setpoint, READWRITE, uint16_t) \
X(0x14, coil_5_setpoint, READWRITE, uint16_t) \
X(0x15, coil_6_setpoint, READWRITE, uint16_t) \
X(0x16, coil_7_setpoint, READWRITE, uint16_t) \
X(0x17, coil_8_setpoint, READWRITE, uint16_t) \
X(0x18, coil_9_setpoint, READWRITE, uint16_t) \
X(0x20, coil_1_current_reading, READONLY, uint16_t) \
X(0x21, coil_2_current_reading, READONLY, uint16_t) \
X(0x22, coil_3_current_reading, READONLY, uint16_t) \
X(0x23, coil_4_current_reading, READONLY, uint16_t) \
X(0x24, coil_5_current_reading, READONLY, uint16_t) \
X(0x25, coil_6_current_reading, READONLY, uint16_t) \
X(0x26, coil_7_current_reading, READONLY, uint16_t) \
X(0x27, coil_8_current_reading, READONLY, uint16_t) \
X(0x28, coil_9_current_reading, READONLY, uint16_t) \
X(0x30, coil_1_temp, READONLY, int16_t) \
X(0x31, coil_2_temp, READONLY, int16_t) \
X(0x32, coil_3_temp, READONLY, int16_t) \
X(0x33, coil_4_temp, READONLY, int16_t) \
X(0x34, coil_5_temp, READONLY, int16_t) \
X(0x35, coil_6_temp, READONLY, int16_t) \
X(0x36, coil_7_temp, READONLY, int16_t) \
X(0x37, coil_8_temp, READONLY, int16_t) \
X(0x38, coil_9_temp, READONLY, int16_t) \
X(0x40, coil_1_estimated_resistance, READONLY, int16_t) \
X(0x41, coil_2_estimated_resistance, READONLY, int16_t) \
X(0x42, coil_3_estimated_resistance, READONLY, int16_t) \
X(0x43, coil_4_estimated_resistance, READONLY, int16_t) \
X(0x44, coil_5_estimated_resistance, READONLY, int16_t) \
X(0x45, coil_6_estimated_resistance, READONLY, int16_t) \
X(0x46, coil_7_estimated_resistance, READONLY, int16_t) \
X(0x47, coil_8_estimated_resistance, READONLY, int16_t) \
X(0x48, coil_9_estimated_resistance, READONLY, int16_t) \
\
\
X(0xC0, build_number, READONLY, uint16_t) \
X(0xC1, mcu_temp, READONLY, uint16_t) \
X(0xC2, main_loop_freq, READONLY, uint32_t) \

#endif /* INC_TILE_REG_XMACRO_H_ */
