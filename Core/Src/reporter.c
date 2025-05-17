/*
 * reporter.c
 *
 *  Created on: Apr 13, 2025
 *      Author: cyfin
 */


#include "reporter.h"
#include <stdint.h>
#include <string.h>
#include "tusb.h"
#include "tile_data.h"

#define TOTAL_ITEMS 47
uint8_t iter_rep_tile = 0;
uint8_t iter_rep_item = 0;

void Reporter_IterativeReportAllTiles() {
    char buffer[64];

    // make sure we have at least 64 bytes in the vendor buffer and the vendor interface is active
    if (!tud_vendor_mounted() || tud_vendor_write_available() < 64) {
		return;
	}

    int addr = iter_rep_tile;

    switch (iter_rep_item) {
        // --- Flags (bitfields) ---
        case 0: snprintf(buffer, sizeof(buffer), "alive %s", tile_data[addr].slave_status.flags.alive ? "true" : "false"); break;
        case 1: snprintf(buffer, sizeof(buffer), "arm_ready %s", tile_data[addr].slave_status.flags.arm_ready ? "true" : "false"); break;
        case 2: snprintf(buffer, sizeof(buffer), "arm_active %s", tile_data[addr].slave_status.flags.arm_active ? "true" : "false"); break;
        case 3: snprintf(buffer, sizeof(buffer), "coils_nonzero %s", tile_data[addr].slave_status.flags.coils_nonzero ? "true" : "false"); break;

        case 4: snprintf(buffer, sizeof(buffer), "temp_fault %s", tile_data[addr].slave_faults.flags.temp_fault ? "true" : "false"); break;
        case 5: snprintf(buffer, sizeof(buffer), "current_spike_fault %s", tile_data[addr].slave_faults.flags.current_spike_fault ? "true" : "false"); break;
        case 6: snprintf(buffer, sizeof(buffer), "vsense_fault %s", tile_data[addr].slave_faults.flags.vsense_fault ? "true" : "false"); break;
        case 7: snprintf(buffer, sizeof(buffer), "invalid_value_fault %s", tile_data[addr].slave_faults.flags.invalid_value_fault ? "true" : "false"); break;

        case 8: snprintf(buffer, sizeof(buffer), "global_arm %s", tile_data[addr].master_status.flags.global_arm ? "true" : "false"); break;
        case 9: snprintf(buffer, sizeof(buffer), "global_fault_clear %s", tile_data[addr].master_status.flags.global_fault_clear ? "true" : "false"); break;

        case 10: snprintf(buffer, sizeof(buffer), "identify %s", tile_data[addr].slave_settings.flags.identify ? "true" : "false"); break;
        case 11: snprintf(buffer, sizeof(buffer), "local_fault_clear %s", tile_data[addr].slave_settings.flags.local_fault_clear ? "true" : "false"); break;

        // --- Float and Scaled Numeric Values ---
        case 12: snprintf(buffer, sizeof(buffer), "v_sense_5 %.3f", tile_data[addr].v_sense_5); break;
        case 13: snprintf(buffer, sizeof(buffer), "v_sense_12 %.3f", tile_data[addr].v_sense_12); break;
        case 14: snprintf(buffer, sizeof(buffer), "v_sense_hv %.3f", tile_data[addr].v_sense_hv); break;
        case 15: snprintf(buffer, sizeof(buffer), "mcu_temp %.3f", tile_data[addr].mcu_temp / 100.0f); break;

        // I2C addresses
        case 16: snprintf(buffer, sizeof(buffer), "adj_west_addr %d", tile_data[addr].adj_west_addr); break;
        case 17: snprintf(buffer, sizeof(buffer), "adj_north_addr %d", tile_data[addr].adj_north_addr); break;
        case 18: snprintf(buffer, sizeof(buffer), "adj_east_addr %d", tile_data[addr].adj_east_addr); break;
        case 19: snprintf(buffer, sizeof(buffer), "adj_south_addr %d", tile_data[addr].adj_south_addr); break;

        // Coil setpoints (mA → A)
        case 20: snprintf(buffer, sizeof(buffer), "coil_1_setpoint %.3f", tile_data[addr].coil_1_setpoint / 1000.0f); break;
        case 21: snprintf(buffer, sizeof(buffer), "coil_2_setpoint %.3f", tile_data[addr].coil_2_setpoint / 1000.0f); break;
        case 22: snprintf(buffer, sizeof(buffer), "coil_3_setpoint %.3f", tile_data[addr].coil_3_setpoint / 1000.0f); break;
        case 23: snprintf(buffer, sizeof(buffer), "coil_4_setpoint %.3f", tile_data[addr].coil_4_setpoint / 1000.0f); break;
        case 24: snprintf(buffer, sizeof(buffer), "coil_5_setpoint %.3f", tile_data[addr].coil_5_setpoint / 1000.0f); break;
        case 25: snprintf(buffer, sizeof(buffer), "coil_6_setpoint %.3f", tile_data[addr].coil_6_setpoint / 1000.0f); break;
        case 26: snprintf(buffer, sizeof(buffer), "coil_7_setpoint %.3f", tile_data[addr].coil_7_setpoint / 1000.0f); break;
        case 27: snprintf(buffer, sizeof(buffer), "coil_8_setpoint %.3f", tile_data[addr].coil_8_setpoint / 1000.0f); break;
        case 28: snprintf(buffer, sizeof(buffer), "coil_9_setpoint %.3f", tile_data[addr].coil_9_setpoint / 1000.0f); break;

        // Coil current readings (mA → A)
        case 29: snprintf(buffer, sizeof(buffer), "coil_1_current_reading %.3f", tile_data[addr].coil_1_current_reading / 1000.0f); break;
        case 30: snprintf(buffer, sizeof(buffer), "coil_2_current_reading %.3f", tile_data[addr].coil_2_current_reading / 1000.0f); break;
        case 31: snprintf(buffer, sizeof(buffer), "coil_3_current_reading %.3f", tile_data[addr].coil_3_current_reading / 1000.0f); break;
        case 32: snprintf(buffer, sizeof(buffer), "coil_4_current_reading %.3f", tile_data[addr].coil_4_current_reading / 1000.0f); break;
        case 33: snprintf(buffer, sizeof(buffer), "coil_5_current_reading %.3f", tile_data[addr].coil_5_current_reading / 1000.0f); break;
        case 34: snprintf(buffer, sizeof(buffer), "coil_6_current_reading %.3f", tile_data[addr].coil_6_current_reading / 1000.0f); break;
        case 35: snprintf(buffer, sizeof(buffer), "coil_7_current_reading %.3f", tile_data[addr].coil_7_current_reading / 1000.0f); break;
        case 36: snprintf(buffer, sizeof(buffer), "coil_8_current_reading %.3f", tile_data[addr].coil_8_current_reading / 1000.0f); break;
        case 37: snprintf(buffer, sizeof(buffer), "coil_9_current_reading %.3f", tile_data[addr].coil_9_current_reading / 1000.0f); break;

        // Coil temps (ºC/100)
        case 38: snprintf(buffer, sizeof(buffer), "coil_1_temp %.3f", tile_data[addr].coil_1_temp / 100.0f); break;
        case 39: snprintf(buffer, sizeof(buffer), "coil_2_temp %.3f", tile_data[addr].coil_2_temp / 100.0f); break;
        case 40: snprintf(buffer, sizeof(buffer), "coil_3_temp %.3f", tile_data[addr].coil_3_temp / 100.0f); break;
        case 41: snprintf(buffer, sizeof(buffer), "coil_4_temp %.3f", tile_data[addr].coil_4_temp / 100.0f); break;
        case 42: snprintf(buffer, sizeof(buffer), "coil_5_temp %.3f", tile_data[addr].coil_5_temp / 100.0f); break;
        case 43: snprintf(buffer, sizeof(buffer), "coil_6_temp %.3f", tile_data[addr].coil_6_temp / 100.0f); break;
        case 44: snprintf(buffer, sizeof(buffer), "coil_7_temp %.3f", tile_data[addr].coil_7_temp / 100.0f); break;
        case 45: snprintf(buffer, sizeof(buffer), "coil_8_temp %.3f", tile_data[addr].coil_8_temp / 100.0f); break;
        case 46: snprintf(buffer, sizeof(buffer), "coil_9_temp %.3f", tile_data[addr].coil_9_temp / 100.0f); break;

        default: return;
    }

    char buffer_complete[64];
    snprintf(buffer_complete, sizeof(buffer_complete), "tiles %d %s\n", addr, buffer);
    tud_vendor_write(buffer_complete, strlen(buffer_complete));
//    tud_vendor_write_flush();

    // increment item
    if (++iter_rep_item >= TOTAL_ITEMS) {
		iter_rep_item = 0;
		if (++iter_rep_tile >= MAX_TILES) {
			iter_rep_tile = 0;
		}
	}

}
