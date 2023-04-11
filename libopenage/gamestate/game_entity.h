// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "coord/phys.h"
#include "gamestate/types.h"
#include "util/path.h"

namespace openage {

namespace renderer::world {
class WorldRenderEntity;
}

namespace gamestate {

/**
 * Entity for a "physical" thing (unit, building, etc.) in the game world.
 */
class GameEntity {
public:
	GameEntity(const entity_id_t id,
	           coord::phys3 pos,
	           util::Path &texture_path);
	~GameEntity() = default;

	/**
     * Get the unique identifier of this entity.
     *
     * @return Unique identifier.
     */
	entity_id_t get_id() const;

	/**
	 * Set the current render entity.
	 *
	 * @param entity New render entity.
	 */
	void set_render_entity(const std::shared_ptr<renderer::world::WorldRenderEntity> &entity);

private:
	// test connection to renderer
	void push_to_render();

	// Unique identifier
	entity_id_t id;
	// position in the game world
	coord::phys3 pos;
	// path to a texture
	util::Path texture_path;

	// render entity for pushing updates to
	std::shared_ptr<renderer::world::WorldRenderEntity> render_entity;
};

} // namespace gamestate
} // namespace openage
