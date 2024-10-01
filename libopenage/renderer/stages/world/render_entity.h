// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <string>

#include <eigen3/Eigen/Dense>

#include "coord/phys.h"
#include "coord/scene.h"
#include "curve/continuous.h"
#include "curve/discrete.h"
#include "curve/segmented.h"
#include "time/time.h"


namespace openage::renderer::world {

/**
 * Render entity for pushing updates to the World renderer.
 */
class WorldRenderEntity {
public:
	WorldRenderEntity();
	~WorldRenderEntity() = default;

	/**
	 * Update the render entity with information from the gamestate.
	 *
	 * Updating the render entity with this method is thread-safe.
	 *
	 * @param ref_id Game entity ID.
	 * @param position Position of the game entity inside the game world.
	 * @param angle Angle of the game entity inside the game world.
	 * @param animation_path Path to the animation definition.
	 * @param time Simulation time of the update.
	 */
	void update(const uint32_t ref_id,
	            const curve::Continuous<coord::phys3> &position,
	            const curve::Segmented<coord::phys_angle_t> &angle,
	            const std::string animation_path,
	            const time::time_t time = 0.0);

	/**
	 * This function is for DEBUGGING and should not be used.
	 *
	 * Update the render entity with information from the gamestate.
	 *
	 * Updating the render entity with this method is thread-safe.
	 *
	 * @param ref_id Game entity ID.
	 * @param position Position of the game entity inside the game world.
	 * @param animation_path Path to the animation definition.
	 * @param time Simulation time of the update.
	 */
	void update(const uint32_t ref_id,
	            const coord::phys3 position,
	            const std::string animation_path,
	            const time::time_t time = 0.0);

	/**
	 * Get the ID of the corresponding game entity.
	 *
	 * Accessing the game entity ID is thread-safe.
	 *
	 * @return Game entity ID.
	 */
	uint32_t get_id();

	/**
	 * Get the position of the entity inside the game world.
	 *
	 * Accessing the position curve REQUIRES a read lock on the render entity
	 * (using \p get_read_lock()) to ensure thread safety.
	 *
	 * @return Position curve of the entity.
	 */
	const curve::Continuous<coord::scene3> &get_position();

	/**
	 * Get the angle of the entity inside the game world.
	 *
	 * Accessing the angle curve REQUIRES a read lock on the render entity
	 * (using \p get_read_lock()) to ensure thread safety.
	 *
	 * @return Angle curve of the entity.
	 */
	const curve::Segmented<coord::phys_angle_t> &get_angle();

	/**
	 * Get the animation definition path.
	 *
	 * Accessing the animation path curve requires a read lock on the render entity
	 * (using \p get_read_lock()) to ensure thread safety.
	 *
	 * @return Path to the animation definition file.
	 */
	const curve::Discrete<std::string> &get_animation_path();

	/**
	 * Get the time of the last update.
	 *
	 * Accessing the update time is thread-safe.
	 *
	 * @return Time of last update.
	 */
	time::time_t get_update_time();

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

	/**
	 * Get a shared lock for thread-safe reading from the render entity.
	 *
	 * The caller is responsible for unlocking the mutex after reading.
	 *
	 * @return Lock for the render entity.
	 */
	std::shared_lock<std::shared_mutex> get_read_lock();

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
	curve::Continuous<coord::scene3> position;

	/**
	 * Angle of the entity inside the game world.
	 */
	curve::Segmented<coord::phys_angle_t> angle;

	/**
	 * Path to the animation definition file.
	 */
	curve::Discrete<std::string> animation_path;

	/**
	 * Time of the last update call.
	 */
	time::time_t last_update;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};
} // namespace openage::renderer::world
