// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "util/path.h"
#include "util/vector.h"

namespace openage::gamestate {

class GameEntity;
class GameState;

/**
 * Creates game entities that contain data of objects inside the game world.
 */
class EntityFactory {
public:
	/**
     * Create a new entity factory for game entities.
     *
     * @param state State of the game.
     */
	EntityFactory(const std::shared_ptr<GameState> &state);
	~EntityFactory() = default;

	/**
     * Create a new game entity and register it at the gamestate.
     *
     * @return New game entity.
     */
	std::shared_ptr<GameEntity> add_game_entity(util::Vector3f pos,
	                                            util::Path &texture_path);

private:
	/**
     * State of the current game.
     */
	std::shared_ptr<GameState> state;
};
} // namespace openage::gamestate
