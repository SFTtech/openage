// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "coord/phys.h"

/**
 * Hardcoded definitions for parameters used in the gamestate.
 *
 * May be moved to configuration files in the future.
 */
namespace openage::gamestate {

/**
 * Origin point of the game world.
 */
constexpr coord::phys3 WORLD_ORIGIN = coord::phys3{0, 0, 0};

} // namespace openage::gamestate
