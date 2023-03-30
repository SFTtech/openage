// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "util/path.h"
#include "util/vector.h"

namespace openage {

namespace renderer {
class RenderFactory;
}

namespace gamestate {
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

	/**
	 * Attach a renderer which enables graphical display options for all ingame entities.
	 *
	 * @param render_factory Factory for creating connector objects for gamestate->renderer
	 *                       communication.
	 */
	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

private:
	/**
     * State of the current game.
     */
	std::shared_ptr<GameState> state;

	/**
	 * Factory for creating connector objects to the renderer which make game entities displayable.
	 */
	std::shared_ptr<renderer::RenderFactory> render_factory;
};
} // namespace gamestate
} // namespace openage
