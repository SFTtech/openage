// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"
#include "util/vector.h"

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
	GameEntity(const uint32_t id,
	           util::Vector3f pos,
	           util::Path &texture_path);
	~GameEntity() = default;

	/**
	 * Set the current render entity.
	 *
	 * @param render_entity New render entity.
	 */
	void set_render_entity(const std::shared_ptr<renderer::world::WorldRenderEntity> &entity);

private:
	// test connection to renderer
	void push_to_render();

	// Unique identifier
	uint32_t id;
	// position in the game world
	util::Vector3f pos;
	// path to a texture
	util::Path texture_path;

	// render entity for pushing updates to
	std::shared_ptr<renderer::world::WorldRenderEntity> render_entity;
};

} // namespace gamestate
} // namespace openage
