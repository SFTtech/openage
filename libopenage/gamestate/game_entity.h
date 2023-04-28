// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>

#include "coord/phys.h"
#include "gamestate/component/component_type.h"
#include "gamestate/types.h"
#include "util/path.h"

namespace openage {

namespace renderer::world {
class WorldRenderEntity;
}

namespace gamestate {

namespace component {
class Component;
}

/**
 * Entity for a "physical" thing (unit, building, etc.) in the game world.
 */
class GameEntity {
public:
	/**
     * Create a new game entity.
     *
     * @param id Unique identifier.
     * @param initial_pos Initial position in the game world.
     * @param texture_path Path to the texture.
     */
	GameEntity(entity_id_t id,
	           coord::phys3 &initial_pos,
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

	/**
     * Add a component to this entity.
     *
     * @param component Component to add.
     */
	void add_component(const std::shared_ptr<component::Component> &component);

	// test connection to renderer
	void push_to_render();

private:
	// Unique identifier
	const entity_id_t id;
	// position in the game world
	coord::phys3 pos;
	// path to a texture
	util::Path texture_path;

	/**
     * Data components.
     */
	std::unordered_map<component::component_t, std::shared_ptr<component::Component>> components;

	// render entity for pushing updates to
	std::shared_ptr<renderer::world::WorldRenderEntity> render_entity;
};

} // namespace gamestate
} // namespace openage
