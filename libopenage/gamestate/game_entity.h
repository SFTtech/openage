// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>

#include "coord/phys.h"
#include "gamestate/component/types.h"
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
     * @param animation_path Path to the texture.
     */
	GameEntity(entity_id_t id,
	           util::Path &animation_path);

	/**
     * Create a new game entity.
     *
     * @param id Unique identifier.
     */
	GameEntity(entity_id_t id);

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
	// path to a texture
	util::Path animation_path;

	/**
     * Data components.
     */
	std::unordered_map<component::component_t, std::shared_ptr<component::Component>> components;

	/**
	 * Render entity for pushing updates to the renderer. Can be \p nullptr.
	 */
	std::shared_ptr<renderer::world::WorldRenderEntity> render_entity;
};

} // namespace gamestate
} // namespace openage
