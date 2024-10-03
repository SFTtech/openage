// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

#include "coord/pixel.h"
#include "curve/continuous.h"
#include "renderer/stages/render_entity.h"


namespace openage::renderer::hud {

/**
 * Render entity for pushing drag selection updates to the HUD renderer.
 */
class DragRenderEntity final : public renderer::RenderEntity {
public:
	/**
	 * Create a new render entity for drag selection in the HUD.
	 *
	 * @param drag_start Position of the start corner.
	 */
	DragRenderEntity(const coord::input drag_start);
	~DragRenderEntity() = default;

	/**
	 * Update the render entity with information from the gamestate
	 * or input system.
	 *
	 * Updating the render entity with this method is thread-safe.
	 *
	 * @param drag_pos Position of the dragged corner.
	 * @param time Current simulation time.
	 */
	void update(const coord::input drag_pos,
	            const time::time_t time = 0.0);

	/**
	 * Get the position of the dragged corner.
	 *
	 * Accessing the drag position curve REQUIRES a read lock on the render entity
	 * (using \p get_read_lock()) to ensure thread safety.
	 *
	 * @return Coordinates of the dragged corner.
	 */
	const curve::Continuous<coord::input> &get_drag_pos();

	/**
	 * Get the position of the start corner.
	 *
	 * Accessing the drag start is thread-safe.
	 *
	 * @return Coordinates of the start corner.
	 */
	const coord::input get_drag_start();

private:
	/**
	 * Position of the dragged corner.
	 */
	curve::Continuous<coord::input> drag_pos;

	/**
	 * Position of the start corner.
	 */
	coord::input drag_start;
};
} // namespace openage::renderer::hud
