// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "coord/phys.h"
#include "gamestate/types.h"


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

/**
 * Starting point for entity IDs.
 *
 * IDs 0-99 are reserved.
 */
constexpr entity_id_t GAME_ENTITY_ID_START = 100;

/**
 * Starting point for player IDs.
 *
 * ID 0 is reserved.
 */
constexpr player_id_t PLAYER_ID_START = 1;

} // namespace openage::gamestate
