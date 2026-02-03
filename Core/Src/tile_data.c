/*
 * tile_data.c
 *
 *  Created on: May 17, 2025
 *      Author: cyfin
 */

#include "tile_data.h"
#include "mt2_types.h"
#include "tile_reg_xmacro.h"
#include "stm32h5xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include "can.h"
#include "adc.h"

#include "tile_data.h"

MT2_Slave_Data tile_data[MAX_TILES];

uint32_t tile_last_seen[MAX_TILES];

TileData_Coordinates tile_coordinates[MAX_TILES];

// array to store setpoints for all 9 coils on all tiles
uint16_t coil_setpoints[MAX_TILES][9];

#define COIL_SETPOINT_START_ADDR 0x10
#define TILE_GLOBAL_STATE_REG 0x06
#define TILE_MASTER_V_SENSE_HV_REG 0x0B
#define TILE_SETTINGS_REG 0x07
#define SETPOINT_MESSAGE_PRIORITY 1

int TileData_Write(uint8_t addr, uint8_t reg, void *data, uint16_t size) {
	if (addr >= MAX_TILES) {
		return -1; // Invalid address
	}

#define X(_reg, _name, _access, _type) \
	if (reg == _reg && size == sizeof(_type)) { \
		memcpy(&tile_data[addr]._name, data, sizeof(_type)); \
		return 0; \
	}

	TILE_REG_XMACRO

#undef X

	return -2; // Invalid register or size mismatch
}

int TileData_Read(uint8_t addr, uint8_t reg, void *data, uint16_t *size) {
	if (addr >= MAX_TILES) {
		return -1; // Invalid address
	}

#define X(_reg, _name, _access, _type) \
	if (reg == _reg) { \
		memcpy(data, &tile_data[addr]._name, sizeof(_type)); \
		if (size) { \
			*size = sizeof(_type); \
		} \
		return 0; \
	}
	TILE_REG_XMACRO
#undef X
	return -2; // Invalid register or size mismatch
}

void TileData_JustHeardFrom(uint8_t addr) {
	if (addr < MAX_TILES) {
		tile_last_seen[addr] = HAL_GetTick();
	}
}

void TileData_MarkInactiveTiles() {
	for (uint8_t i = 0; i < MAX_TILES; i++) {
		if (tile_last_seen[i] + ALIVE_TIMEOUT < HAL_GetTick()) {
			tile_data[i].slave_status.flags.alive = 0;
		}
	}
}

// Turn on ID light for specified tile, off for all others
void TileData_IdentifyTile(uint8_t addr) {
	if (addr >= MAX_TILES) {
		return;
	}
	// for each alive tile, send its current settings value with ID flag set appropriately
	for (uint8_t i = 1; i < MAX_TILES; i++) {
		if (tile_data[i].slave_status.flags.alive) {
			MT2_Slave_Settings settings = tile_data[i].slave_settings;
			if (i == addr) {
				settings.flags.identify = 1;
			} else {
				settings.flags.identify = 0;
			}
			uint8_t data[sizeof(MT2_Slave_Settings) + 1];
			data[0] = TILE_SETTINGS_REG;
			memcpy(&data[1], &settings, sizeof(MT2_Slave_Settings));
			CAN_SendMessage((SETPOINT_MESSAGE_PRIORITY << 8) | i, data, sizeof(data));
		}
	}
}

// Map dimensions (set by TileData_ComputeCoordinates)
uint8_t tile_map_width;
uint8_t tile_map_height;

// Temporary buffer for atomic update
static TileData_Coordinates tmp_coords[MAX_TILES];
static uint8_t tmp_map_width;
static uint8_t tmp_map_height;

// Iterative DFS stack and visited flags
static uint8_t dfs_stack_id[MAX_TILES];
static int8_t dfs_stack_x[MAX_TILES];
static int8_t dfs_stack_y[MAX_TILES];
static uint8_t dfs_visited[MAX_TILES];

void TileData_ComputeCoordinates(void) {
	// Clear visited and temporary coords
	for (uint8_t i = 0; i < MAX_TILES; i++) {
		dfs_visited[i] = 0;
		tmp_coords[i].x = TILE_MAP_SENTINEL;  // sentinel for unvisited
		tmp_coords[i].y = TILE_MAP_SENTINEL;
	}
	tmp_map_width = 0;
	tmp_map_height = 0;

	// Find first alive tile (skip addr 0)
	uint8_t start_id = MAX_TILES;
	for (uint8_t i = 1; i < MAX_TILES; i++) {
		if (tile_data[i].slave_status.flags.alive) {
			start_id = i;
			break;
		}
	}
	if (start_id == MAX_TILES) {
		// No alive tiles: publish zeros
		__disable_irq();
		tile_map_width = 0;
		tile_map_height = 0;
		memset(tile_coordinates, TILE_MAP_SENTINEL, sizeof(tile_coordinates));
		__enable_irq();
		return;
	}

	// Push start onto stack
	int top = 0;
	dfs_stack_id[top] = start_id;
	dfs_stack_x[top] = 0;
	dfs_stack_y[top] = 0;
	dfs_visited[start_id] = 1;
	top++;

	int minX = 0, minY = 0, maxX = 0, maxY = 0;
	bool first = true;

	while (top > 0) {
		// Pop
		top--;
		uint8_t id = dfs_stack_id[top];
		int8_t x = dfs_stack_x[top];
		int8_t y = dfs_stack_y[top];

		// Record in tmp buffer
		tmp_coords[id].x = x;
		tmp_coords[id].y = y;

		// Update bounds
		if (first) {
			minX = maxX = x;
			minY = maxY = y;
			first = false;
		} else {
			if (x < minX) minX = x;
			if (x > maxX) maxX = x;
			if (y < minY) minY = y;
			if (y > maxY) maxY = y;
		}

		// Explore neighbors in order: North, East, South, West
		MT2_Slave_Data *t = &tile_data[id];
		uint8_t nbr;
		// North
		nbr = t->adj_north_addr;
		if (nbr && nbr < MAX_TILES && !dfs_visited[nbr] && tile_data[nbr].slave_status.flags.alive) {
			dfs_stack_id[top] = nbr;
			dfs_stack_x[top] = x;
			dfs_stack_y[top] = y - 1;
			dfs_visited[nbr] = 1;
			top++;
		}
		// East
		nbr = t->adj_east_addr;
		if (nbr && nbr < MAX_TILES && !dfs_visited[nbr] && tile_data[nbr].slave_status.flags.alive) {
			dfs_stack_id[top] = nbr;
			dfs_stack_x[top] = x + 1;
			dfs_stack_y[top] = y;
			dfs_visited[nbr] = 1;
			top++;
		}
		// South
		nbr = t->adj_south_addr;
		if (nbr && nbr < MAX_TILES && !dfs_visited[nbr] && tile_data[nbr].slave_status.flags.alive) {
			dfs_stack_id[top] = nbr;
			dfs_stack_x[top] = x;
			dfs_stack_y[top] = y + 1;
			dfs_visited[nbr] = 1;
			top++;
		}
		// West
		nbr = t->adj_west_addr;
		if (nbr && nbr < MAX_TILES && !dfs_visited[nbr] && tile_data[nbr].slave_status.flags.alive) {
			dfs_stack_id[top] = nbr;
			dfs_stack_x[top] = x - 1;
			dfs_stack_y[top] = y;
			dfs_visited[nbr] = 1;
			top++;
		}
	}

	// Shift coords so all >= 0 and compute final map size
	int8_t shiftX = -minX;
	int8_t shiftY = -minY;
	for (uint8_t i = 0; i < MAX_TILES; i++) {
		if (tmp_coords[i].x != TILE_MAP_SENTINEL) {
			tmp_coords[i].x += shiftX;
			tmp_coords[i].y += shiftY;
			if (tmp_coords[i].x + 1 > tmp_map_width) tmp_map_width = tmp_coords[i].x + 1;
			if (tmp_coords[i].y + 1 > tmp_map_height) tmp_map_height = tmp_coords[i].y + 1;
		}
	}

	// Publish under critical section
	__disable_irq();
	memcpy(tile_coordinates, tmp_coords, sizeof(tmp_coords));
	tile_map_width = tmp_map_width;
	tile_map_height = tmp_map_height;
	__enable_irq();
}

int TileData_AssignSetpoint(uint8_t x, uint8_t y, uint16_t setpoint) {
	// No map available?
	if (tile_map_width == 0 || tile_map_height == 0) {
		return -1;
	}
	// Determine which tile cell this lies in
	uint8_t tileGridX = x / 3;
	uint8_t tileGridY = y / 3;
	uint8_t localX = x % 3;
	uint8_t localY = y % 3;
	// Quick bounds check
	if (tileGridX >= tile_map_width || tileGridY >= tile_map_height) {
		return -2;
	}
	// Find matching tile
	for (uint8_t id = 1; id < MAX_TILES; id++) {
		if (tile_coordinates[id].x == tileGridX && tile_coordinates[id].y == tileGridY) {
			uint8_t coilIndex = localY * 3 + localX;
			// Write the setpoint
			coil_setpoints[id][coilIndex] = setpoint;
			return 0;
		}
	}
	return -3;
}

#define ITER_SETPOINT_INTERVAL 15 // ms
#define ITER_GLOBAL_INTERVAL 50 // ms

uint8_t iter_setpoint_tile_id = 1;
uint8_t iter_setpoint_coil_index = 0;
uint8_t iter_setpoint_reached_end = 0; // flag to indicate if we reached the end of the setpoints
uint32_t iter_setpoint_last_started = 0; // last time we started sending setpoints
uint32_t iter_global_last = 0; // last time we started the global iteration

extern MT2_Global_State global_state; // global state to send


// Gemini wrote this
int TileData_IterativeSendSetpoints(void) {
    // 1. IN-PLACE FIX: Static cache to remember what we sent last time.
    // Dimensions based on MAX_TILES and the fact that you check 'index >= 9'.
    // Static variables are zero-initialized by default.
    static uint16_t last_sent_setpoints[MAX_TILES][9];

    if (HAL_GetTick() - iter_global_last > ITER_GLOBAL_INTERVAL) {
        uint8_t message[8];
        message[0] = TILE_GLOBAL_STATE_REG; // register address
        memcpy(&message[1], &global_state, sizeof(global_state));
        HAL_StatusTypeDef result = CAN_SendMessage((SETPOINT_MESSAGE_PRIORITY << 8) | 0, message, sizeof(global_state)+1);
        if (result != HAL_OK) {
            return -2; // Send failed
        } else if (global_state.flags.global_fault_clear) { // if we successfully sent a clear flag
            global_state.flags.global_fault_clear = 0;
        }
        message[0] = TILE_MASTER_V_SENSE_HV_REG; // register address
        memcpy(&message[1], &v_sense_hv, sizeof(v_sense_hv));
        result = CAN_SendMessage((SETPOINT_MESSAGE_PRIORITY << 8) | 0, message, sizeof(v_sense_hv)+1);
        iter_global_last = HAL_GetTick();
    }

    if (iter_setpoint_reached_end) {
        if (HAL_GetTick() - iter_setpoint_last_started < ITER_SETPOINT_INTERVAL) {
            return 1; // wait for next interval
        } else {
            iter_setpoint_last_started = HAL_GetTick();
            iter_setpoint_reached_end = 0; // reset the flag
        }
    }

    // 2. IN-PLACE FIX: Loop until we find something to send or reach the end.
    // This prevents returning 0 (success) without actually sending anything,
    // which would require the caller to call this function hundreds of times just to skip unchanged values.
    while (1) {
        for (int i = 0; i < MAX_TILES; i++) {
            if (iter_setpoint_coil_index >= 9) {
                iter_setpoint_coil_index = 0;
                iter_setpoint_tile_id++;
            }
            if (iter_setpoint_tile_id >= MAX_TILES) {
                iter_setpoint_tile_id = 1; // skip addr 0
                iter_setpoint_reached_end = 1; // reached the end of the setpoints
                return 1; // No more setpoints to send
            }
            if (tile_data[iter_setpoint_tile_id].slave_status.flags.alive) {
                break;
            }
            iter_setpoint_tile_id++;
        }

        if (tile_data[iter_setpoint_tile_id].slave_status.flags.alive == 0) {
            return -1; // No alive tiles
        }

        uint16_t setpoint = coil_setpoints[iter_setpoint_tile_id][iter_setpoint_coil_index];

        // 3. IN-PLACE FIX: Check if value changed
        if (last_sent_setpoints[iter_setpoint_tile_id][iter_setpoint_coil_index] != setpoint) {

            // -- Value Changed: Send Message --

            uint8_t addr = iter_setpoint_tile_id;
            uint8_t message[3];
            // if (setpoint > 3000) {
            //  setpoint = 0;
            // }
            message[0] = COIL_SETPOINT_START_ADDR + iter_setpoint_coil_index; // register address
            memcpy(&message[1], &setpoint, sizeof(setpoint)); // setpoint value

            // Send the message
            HAL_StatusTypeDef result = CAN_SendMessage((SETPOINT_MESSAGE_PRIORITY << 8) | addr, message, sizeof(message));
            if (result != HAL_OK) {
                return -2; // Send failed
            }

            // Update the cache so we don't send this again next time
            last_sent_setpoints[iter_setpoint_tile_id][iter_setpoint_coil_index] = setpoint;

            // Increment coil index
            iter_setpoint_coil_index++;

            return 0; // We did work, return control
        } else {
            // -- Value Unchanged: Skip Send --

            // Increment coil index
            iter_setpoint_coil_index++;

            // Continue the 'while(1)' loop to immediately check the next coil
            // without exiting the function.
            continue;
        }
    }
}
