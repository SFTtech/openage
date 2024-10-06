// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <mutex>
#include <shared_mutex>

#include <eigen3/Eigen/Dense>

#include "time/time.h"


namespace openage::renderer {

/**
 * Interface for render entities that allow pushing updates from game simulation
 * to renderer.
 */
class RenderEntity {
public:
	~RenderEntity() = default;

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

protected:
	/**
	 * Create a new render entity.
	 *
	 * Members are initialized to these values by default:
	 * - \p changed: false
	 * - \p last_update: time::time_t::zero()
	 *
	 * Declared as protected to prevent direct instantiation of this class.
	 */
	RenderEntity();

	/**
	 * Flag for determining if the render entity has been updated by the
	 * corresponding gamestate entity. Set to true every time \p update()
	 * is called.
	 */
	bool changed;

	/**
	 * Time of the last update call.
	 */
	time::time_t last_update;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};
} // namespace openage::renderer
