/*
 * serial_terminal.c
 *
 *  Created on: Jul 20, 2025
 *      Author: cyfin
 */

#include <stdint.h>

//#include "master_reg_xmacro.h"

#include <string.h>

#include "tusb.h"
#include "tile_data.h"

#include "serial_terminal.h"

extern uint8_t hv_active;
extern uint8_t hv_activated_by_cdc;

extern uint8_t tile_map_width;
extern uint8_t tile_map_height;

void SerialTerminal_ReplyOk(char* msg) {
	tud_cdc_write_str("ok : ");
	tud_cdc_write_str(msg);
	tud_cdc_write_str("\n");
	tud_cdc_write_flush();
}

void SerialTerminal_ReplyOkNoMsg() {
	tud_cdc_write_str("ok\n");
	tud_cdc_write_flush();
}

void SerialTerminal_ReplyError(char* msg) {
	tud_cdc_write_str("error : ");
	tud_cdc_write_str(msg);
	tud_cdc_write_str("\n");
	tud_cdc_write_flush();
}

/*

arm
disarm
read_width
read_height
scan_addresses
read_tile_position <addr>
test_led_enable <addr>
test_led_disable <addr>
set_power <row> <col> <power>
get_power <row> <col>
zero_all

 */

void SerialTerminal_HandleCommand(char *str) {
	char command_name[MAX_CDC_MSG_LEN];
	char command_args[MAX_CDC_MSG_LEN];
	// parse the command
	int parsed = sscanf(str, "%s %[^\n]", command_name, command_args);
#define IF_COMMAND_IS(_cmd)  if (strcmp(command_name, _cmd) == 0)

	// TODO: Ensure safety of non-atomic operations

	IF_COMMAND_IS("arm") {
		if (hv_active) {
			SerialTerminal_ReplyOkNoMsg();
			return;
		}
		hv_activated_by_cdc = 1;
		hv_active = 1;
		return;
	}
	IF_COMMAND_IS("disarm") {
		hv_active = 0;
		SerialTerminal_ReplyOkNoMsg();
		return;
	}
	IF_COMMAND_IS("read_width") {
		char buf[16];
		sprintf(buf, "%d", tile_map_width);
		SerialTerminal_ReplyOk(buf);
		return;
	}
	IF_COMMAND_IS("read_height") {
		char buf[16];
		sprintf(buf, "%d", tile_map_height);
		SerialTerminal_ReplyOk(buf);
		return;
	}
	IF_COMMAND_IS("scan_addresses") {
		// iterate through tile data array, and generate space-separated string of addresses that are alive
		char result[MAX_TILES * 8];
		unsigned int pos = 0;
		for (unsigned int i = 1; i < MAX_TILES; i++) {
			if (tile_data[i].slave_status.flags.alive) {
				pos += sprintf(result + pos, "%d ", i);
			}
		}
		if (pos > 0) {
			result[pos - 1] = '\0'; // remove last space
		} else {
			result[0] = '\0'; // no alive tiles
		}
		SerialTerminal_ReplyOk(result);
		return;
	}
	IF_COMMAND_IS("read_tile_position") {
		uint8_t addr;
		if (sscanf(command_args, "%hhu", &addr) != 1 || addr >= MAX_TILES) {
			SerialTerminal_ReplyError("Invalid address");
			return;
		}
		char buf[32];
		sprintf(buf, "%d %d", tile_coordinates[addr].x, tile_coordinates[addr].y);
		SerialTerminal_ReplyOk(buf);
		return;
	}
	IF_COMMAND_IS("test_led_enable") {
		uint8_t addr;
		if (sscanf(command_args, "%hhu", &addr) != 1 || addr >= MAX_TILES) {
			SerialTerminal_ReplyError("Invalid address");
			return;
		}
		// TODO: Implement LED enable command
	}
	IF_COMMAND_IS("test_led_disable") {
		uint8_t addr;
		if (sscanf(command_args, "%hhu", &addr) != 1 || addr >= MAX_TILES) {
			SerialTerminal_ReplyError("Invalid address");
			return;
		}
		// TODO: Implement LED disable command
	}
	IF_COMMAND_IS("set_power") {
		int row;
		int col;
		int power;
		if (sscanf(command_args, "%d %d %d", &row, &col, &power) != 3) {
			SerialTerminal_ReplyError("Invalid arguments");
			return;
		}
		if (row >= tile_map_height*3 || col >= tile_map_width*3) {
			// say "Coordinate out of bounds; map dimensions are <width> x <height>"
			char buf[64];
			sprintf(buf, "Coordinate out of bounds; map dimensions are %d x %d", tile_map_width*3, tile_map_height*3);
			SerialTerminal_ReplyError(buf);
			return;
		}
		int result_code = TileData_AssignSetpoint(col, row, power);
		if (result_code == 0) {
			SerialTerminal_ReplyOkNoMsg();
		} else {
			char buf[64];
			sprintf(buf, "Failed to set power, error code %d", result_code);
			SerialTerminal_ReplyError(buf);
		}
		return;
	}
	IF_COMMAND_IS("get_power") {
		uint8_t row, col;
		if (sscanf(command_args, "%hhu %hhu", &row, &col) != 2 ||
		    row >= tile_map_height || col >= tile_map_width) {
			SerialTerminal_ReplyError("Invalid arguments");
			return;
		}
		uint16_t power = coil_setpoints[col][row];
		char buf[16];
		sprintf(buf, "%d", power);
		SerialTerminal_ReplyOk(buf);
		return;
	}
	IF_COMMAND_IS("zero_all") {
		// zero all setpoints
		memset(coil_setpoints, 0, sizeof(coil_setpoints));
		SerialTerminal_ReplyOkNoMsg();
		return;
	}

	SerialTerminal_ReplyError("Unknown command");
	return;
#undef IF_COMMAND_IS
}
