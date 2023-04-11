// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <shared_mutex>

#include "coord/phys.h"
#include "gamestate/types.h"
#include "util/path.h"

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
     */
	EntityFactory();
	~EntityFactory() = default;

	/**
     * Create a new game entity and register it at the gamestate.
     *
     * @return New game entity.
     */
	std::shared_ptr<GameEntity> add_game_entity(coord::phys3 pos,
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
     * Get a unique ID for creating a game entity.
     *
     * @return Unique ID for a game entity.
     */
	entity_id_t get_next_id();

	/**
     * ID of the next game entity to be created.
     */
	entity_id_t next_id;

	/**
	 * Factory for creating connector objects to the renderer which make game entities displayable.
	 */
	std::shared_ptr<renderer::RenderFactory> render_factory;

	/**
     * Mutex for thread safety.
     */
	std::shared_mutex mutex;
};
} // namespace gamestate
} // namespace openage
