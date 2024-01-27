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
 * Total integrated cost in the integration field.
 */
using integrate_t = uint16_t;

/**
 * Flow field direction value.
 */
using flow_dir_t = uint8_t;

} // namespace openage::path
