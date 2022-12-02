// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Dense>

#include "util/path.h"
#include "util/vector.h"

namespace openage::renderer::world {

class WorldRenderEntity {
public:
	WorldRenderEntity();
	~WorldRenderEntity() = default;

	/**
	 * Update the render entity with informatio from the gamestate.
	 *
	 * @param ref_id Game entity ID.
	 * @param position Position of the game entity inside the game world.
	 * @param texture_path Path to the animation texture.
	 */
	void update(const uint32_t ref_id,
	            const util::Vector3f position,
	            const util::Path texture_path);

	/**
	 * Get the ID of the corresponding game entity.
	 *
	 * @return Game entity ID.
	 */
	uint32_t get_id();

	/**
	 * Get the position of the entity inside the game world.
	 *
	 * @return Position of the entity.
	 */
	const Eigen::Vector3f get_position();

	/**
     * Get the texture path.
     *
     * TODO: Return the animation.
     *
     * @return Path to the texture.
     */
	const util::Path &get_texture_path();

	/**
	 * Check whether the render entity has received new updates from the
	 * gamestate.
	 *
	 * @return true if updates have been received, else false.
	 */
	bool is_changed();

	/**
	 * Clear the update flag by setting it to false.
	 */
	void clear_changed_flag();

private:
	/**
	 * Flag for determining if the render entity has been updated by the
	 * corresponding gamestate entity. Set to true every time \p update()
	 * is called.
	 */
	bool changed;

	/**
	 * ID of the game entity in the gamestate.
	 */
	uint32_t ref_id;

	/**
	 * Position inside the game world.
	 */
	Eigen::Vector3f position;

	/**
	 * Path to the texture.
	 */
	util::Path texture_path;
};
} // namespace openage::renderer::world