// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <cstdint>


namespace openage::path {

/**
 * Path result type.
 */
enum class PathResult {
	/// Path was found.
	FOUND,
	/// Path was not found.
	NOT_FOUND,
};

/**
 * Movement cost in the cost field.
 *
 * TODO: Cost stamps
 *
 * 0: uninitialized
 * 1-254: normal cost
 * 255: impassable
 */
using cost_t = uint8_t;

/**
 * Integrated cost in the integration field.
 */
using integrated_cost_t = uint16_t;

/**
 * Integrated field cell flags.
 */
using integrated_flags_t = uint8_t;

/**
 * Integration field cell value.
 */
struct integrated_t {
	/**
	 * Total integrated cost.
	 */
	integrated_cost_t cost;

	/**
	 * Flags.
	 *
	 * Bit 6: Wave front blocked flag.
	 * Bit 7: Line of sight flag.
	 */
	integrated_flags_t flags;
};

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
 * Flow field cell value.
 *
 * Bit 0: Unused.
 * Bit 1: Wave front blocked flag.
 * Bit 2: Line of sight flag.
 * Bit 3: Pathable flag.
 * Bits 4-7: flow direction.
 */
using flow_t = uint8_t;

/**
 * Grid identifier.
 */
using grid_id_t = size_t;

/**
 * Sector identifier (unique per grid).
 */
using sector_id_t = size_t;

/**
 * Portal identifier (unique per grid).
 */
using portal_id_t = size_t;

} // namespace openage::path
