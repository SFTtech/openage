// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <shared_mutex>
#include <string>

#include "coord/pixel.h"
#include "curve/continuous.h"
#include "time/time.h"


namespace openage::renderer::hud {

/**
 * Render entity for pushing drag selection updates to the HUD renderer.
 */
class HudDragRenderEntity {
public:
	/**
	 * Create a new render entity for drag selection in the HUD.
	 *
	 * @param drag_start Position of the start corner.
	 */
	HudDragRenderEntity(const coord::input drag_start);
	~HudDragRenderEntity() = default;

	/**
	 * Update the render entity with information from the gamestate
	 * or input system.
	 *
	 * @param drag_pos Position of the dragged corner.
	 * @param time Current simulation time.
	 */
	void update(const coord::input drag_pos,
	            const time::time_t time = 0.0);

	/**
	 * Get the time of the last update.
	 *
	 * @return Time of last update.
	 */
	time::time_t get_update_time();

	/**
	 * Get the position of the dragged corner.
	 *
	 * @return Coordinates of the dragged corner.
	 */
	const curve::Continuous<coord::input> &get_drag_pos();

	/**
	 * Get the position of the start corner.
	 *
	 * @return Coordinates of the start corner.
	 */
	const coord::input &get_drag_start();

	/**
	 * Check whether the render entity has received new updates.
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
	 * Time of the last update call.
	 */
	time::time_t last_update;

	/**
	 * Position of the dragged corner.
	 */
	curve::Continuous<coord::input> drag_pos;

	/**
	 * Position of the start corner.
	 */
	coord::input drag_start;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};
} // namespace openage::renderer::hud
