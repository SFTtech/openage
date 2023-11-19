// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <list>
#include <shared_mutex>
#include <string>

#include "time/time.h"


namespace openage::renderer::hud {

class HudRenderEntity {
public:
	HudRenderEntity();
	~HudRenderEntity() = default;

	/**
	 * TODO: Update the render entity with information from the gamestate.
	 */
	void update(const time::time_t time = 0.0);

	/**
	 * Get the time of the last update.
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
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};
} // namespace openage::renderer::hud
