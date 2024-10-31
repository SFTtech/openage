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
constexpr integrated_cost_t INTEGRATED_COST_START = 0;

/**
 * Unreachable value for a cells in the integration grid.
 */
constexpr integrated_cost_t INTEGRATED_COST_UNREACHABLE = std::numeric_limits<integrated_cost_t>::max();

/**
 * Line of sight flag in an integrated_flags_t value.
 */
constexpr integrated_flags_t INTEGRATE_LOS_MASK = 0x20;

/**
 * Target flag in an integrated_flags_t value.
 */
constexpr integrated_flags_t INTEGRATE_TARGET_MASK = 0x40;

/**
 * Found flag in an integrated_flags_t value.
 */
constexpr integrated_flags_t INTEGRATE_FOUND_MASK = 0x02;

/**
 * Wavefront blocked flag in an integrated_flags_t value.
 */
constexpr integrated_flags_t INTEGRATE_WAVEFRONT_BLOCKED_MASK = 0x04;

/**
 * Initial value for a cell in the integration grid.
 */
constexpr integrated_t INTEGRATE_INIT = {INTEGRATED_COST_UNREACHABLE, 0};


/**
 * Initial value for a flow field cell.
 */
constexpr flow_t FLOW_INIT = 0;

/**
 * Mask for the flow direction bits in a flow_t value.
 */
constexpr flow_t FLOW_DIR_MASK = 0x0F;

/**
 * Mask for the flow flag bits in a flow_t value.
 */
constexpr flow_t FLOW_FLAGS_MASK = 0xF0;

/**
 * Pathable flag in a flow_t value.
 */
constexpr flow_t FLOW_PATHABLE_MASK = 0x10;

/**
 * Line of sight flag in a flow_t value.
 */
constexpr flow_t FLOW_LOS_MASK = 0x20;

/**
 * Target flag in a flow_t value.
 */
constexpr flow_t FLOW_TARGET_MASK = 0x40;

} // namespace openage::path
