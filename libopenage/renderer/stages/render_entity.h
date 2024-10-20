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
 *
 * Accessing the render entity from the renderer thread REQUIRES a
 * read lock on the render entity (using \p get_read_lock()) to ensure
 * thread safety.
 */
class RenderEntity {
public:
	~RenderEntity() = default;

	/**
	 * Get the earliest time for which updates are available.
	 *
	 * Render objects should synchronize their state with the render entity
	 * from this time onwards.
	 *
	 * Accessing the update time is thread-safe.
	 *
	 * @return Time of last update.
	 */
	time::time_t get_fetch_time();

	/**
	 * Check whether the render entity has received new updates from the
	 * gamestate.
	 *
	 * Accessing the change flag is thread-safe.
	 *
	 * @return true if updates have been received, else false.
	 */
	bool is_changed();

	/**
	 * Indicate to this entity that its updates have been processed and transfered to the
	 * render object.
	 *
	 * - Clear the update flag by setting it to false.
	 * - Sets the fetch time to \p time::MAX_TIME.
	 *
	 * Accessing this method is thread-safe.
	 */
	void fetch_done();

	/**
	 * Get a shared lock for thread-safe reading from the render entity.
	 *
	 * The caller is responsible for unlocking the mutex after reading.
	 *
	 * @return Lock for the render entity.
	 */
	std::unique_lock<std::shared_mutex> get_read_lock();

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
	 * Time of the last update.
	 */
	time::time_t last_update;

	/**
	 * Earliest time for which updates have been received.
	 *
	 * \p time::TIME_MAX indicates that no updates are available.
	 */
	time::time_t fetch_time;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};
} // namespace openage::renderer
