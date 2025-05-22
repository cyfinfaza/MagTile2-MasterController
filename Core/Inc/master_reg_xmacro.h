/*
 * master_reg_xmacro.h
 *
 *  Created on: May 18, 2025
 *      Author: cyfin
 */

#ifndef INC_MASTER_REG_XMACRO_H_
#define INC_MASTER_REG_XMACRO_H_

#define MASTER_REG_XMACRO \
X(0x10, power_switch_status, READONLY, MT2_Master_PowerSwitchStatus) \
X(0x11, power_system_faults, READONLY, MT2_Master_PowerSystemFaults) \
X(0x12, usb_interface_status, READONLY, MT2_Master_UsbInterfaceStatus) \
X(0x13, global_state, READONLY, MT2_Global_State) \
\
X(0x20, hv_active, READWRITE, uint8_t) \
X(0x21, clear_faults_requested, READWRITE, uint8_t) \
\
X(0x30, mcu_temp, READONLY, int16_t) \
X(0x31, v_sense_5, READONLY, float) \
X(0x32, v_sense_12_in, READONLY, float) \
X(0x33, v_sense_12, READONLY, float) \
X(0x34, v_sense_hv_in, READONLY, float) \
X(0x35, v_sense_hv, READONLY, float) \
X(0x36, i_sense_5, READONLY, float) \
X(0x37, i_sense_12, READONLY, float) \
X(0x38, i_sense_hv, READONLY, float) \

#endif /* INC_MASTER_REG_XMACRO_H_ */
