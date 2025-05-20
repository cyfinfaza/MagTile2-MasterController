/*
 * registry_map.c
 *
 *  Created on: May 16, 2025
 *      Author: cyfin
 */

#include "registry.h"
#include "mt2_types.h"
#include "adc.h"

#define X(addr, name, access, type) extern type name;
    TILE_REG_XMACRO
#undef X

#define REGISTRY_COUNT 255


// Manual entries
const Registry_RegConfig registry_table[REGISTRY_COUNT] = {
#define X(addr, name, access, type) Registry_DEFINE(addr, &name, Registry_##access),
    TILE_REG_XMACRO
#undef X
};

const uint32_t registry_count = REGISTRY_COUNT;
