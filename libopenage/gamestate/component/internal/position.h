// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "coord/phys.h"
#include "curve/continuous.h"
#include "gamestate/component/internal_component.h"
#include "util/fixed_point.h"

namespace openage::gamestate::component {

using angle_t = util::FixedPoint<uint32_t, 16>;

class Position : public InternalComponent {
public:
	/**
     * Create a Position component.
     *
     * @param loop Event loop that all events from the component are registered on.
     * @param initial_pos Initial position at creation time.
     * @param creation_time Ingame creation time of the component.
     */
	Position(const std::shared_ptr<openage::event::Loop> &loop,
	         const coord::phys3 &initial_pos,
	         const curve::time_t &creation_time);

	component_t get_type() const override;

	/**
     * Get the positions in the world coordinate system over time.
     *
     * @return Position curve.
     */
	const curve::Continuous<coord::phys3> &get_positions() const;

	/**
     * Get the directions in degrees over time.
     *
     * @return Direction curve.
     */
	const curve::Continuous<angle_t> &get_angles() const;

private:
	/**
     * Position storage over time.
     */
	curve::Continuous<coord::phys3> position;

	/**
     * Angle the entity is facing over time.
     *
     * Represents degrees in the range [0, 360). At angle 0, the entity is facing
     * towards the camera.
     */
	curve::Continuous<angle_t> angle;
};

} // namespace openage::gamestate::component
