// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <limits>

#include "pathfinding/types.h"


namespace openage::path {

/**
 * Init value for a cells in the cost grid.
 *
 * Should not be used for actual costs.
 */
constexpr cost_t COST_INIT = 0;

/**
 * Minimum possible cost for a passable cell in the cost grid.
 */
constexpr cost_t COST_MIN = 1;

/**
 * Maximum possible cost for a passable cell in the cost grid.
 */
constexpr cost_t COST_MAX = 254;

/**
 * Cost value for an impassable cell in the cost grid.
 */
constexpr cost_t COST_IMPASSABLE = 255;


/**
 * Start value for goal cells.
 */
const integrate_t INTEGRATE_START = 0;

/**
 * Unreachable value for a cells in the integration grid.
 */
constexpr integrate_t INTEGRATE_UNREACHABLE = std::numeric_limits<integrate_t>::max();


/**
 * Flow field direction types.
 *
 * Encoded into the flow_t values.
 */
enum class flow_dir_t : uint8_t {
	NORTH = 0x00,
	NORTH_EAST = 0x01,
	EAST = 0x02,
	SOUTH_EAST = 0x03,
	SOUTH = 0x04,
	SOUTH_WEST = 0x05,
	WEST = 0x06,
	NORTH_WEST = 0x07,
};

/**
 * Mask for the flow direction bits in a flow_t value.
 */
constexpr flow_t FLOW_DIR_MASK = 0x0F;

/**
 * Mask for the pathable flag in a flow_t value.
 */
constexpr flow_t FLOW_PATHABLE = 0x10;

/**
 * Mask for the line of sight flag in a flow_t value.
 */
constexpr flow_t FLOW_LOS = 0x20;

} // namespace openage::path
