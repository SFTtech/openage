// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>


namespace openage::path {

/**
 * Path result type.
 */
enum class PathResult {
	/// Path was found.
	FOUND,
	/// Path was not found.
	NOT_FOUND,
	/// Target is not on grid.
	OUT_OF_BOUNDS,
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
	 * Bit 0-3: Shared flags with the flow field.
	 *  - 0: Unused.
	 *  - 1: Target flag.
	 *  - 2: Line of sight flag.
	 *  - 3: Unused.
	 * Bit 4-7: Integration field specific flags.
	 *  - 4: Unused.
	 *  - 5: Wave front blocked flag.
	 *  - 6: LOS found flag.
	 *  - 7: Unused.
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
 * Bit 1: Target flag.
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

class FlowField;
class IntegrationField;

/**
 * Cache key for accessing the field cache using a portal id and a sector id.
 */
using cache_key_t = std::pair<portal_id_t, sector_id_t>;

/**
 * Returnable field cache entry pair containing an integration field and a flow field.
 */
using field_cache_t = std::pair<std::shared_ptr<IntegrationField>, std::shared_ptr<FlowField>>;

} // namespace openage::path
