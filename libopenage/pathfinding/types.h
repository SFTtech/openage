// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>


namespace openage::path {

/**
 * Movement cost in the cost field.
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
struct integrate_t {
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
 * Flow field cell value.
 *
 * Bit 2: Line of sight flag.
 * Bit 3: Pathable flag.
 * Bits 4-7: flow direction.
 */
using flow_t = uint8_t;

} // namespace openage::path
