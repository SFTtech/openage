// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "coord/phys.h"
#include "curve/continuous.h"
#include "curve/segmented.h"
#include "gamestate/component/internal_component.h"
#include "util/fixed_point.h"

namespace openage::gamestate::component {

class Position : public InternalComponent {
public:
	/**
     * Create a Position component.
     *
     * @param loop Event loop that all events from the component are registered on.
     * @param initial_pos Initial position at creation time.
     * @param creation_time Ingame creation time of the component.
     */
	Position(const std::shared_ptr<openage::event::EventLoop> &loop,
	         const coord::phys3 &initial_pos,
	         const curve::time_t &creation_time);

	/**
     * Create a Position component.
     *
     * @param loop Event loop that all events from the component are registered on.
     */
	Position(const std::shared_ptr<openage::event::EventLoop> &loop);

	component_t get_type() const override;

	/**
     * Get the positions in the world coordinate system over time.
     *
     * @return Position curve.
     */
	const curve::Continuous<coord::phys3> &get_positions() const;

	/**
     * Set the position at a given time.
     *
     * This adds a new keyframe to the position curve.
     *
     * @param time Time at which the position is set.
     * @param pos New position.
     */
	void set_position(const curve::time_t &time, const coord::phys3 &pos);

	/**
     * Get the directions in degrees over time.
     *
     * @return Direction curve.
     */
	const curve::Segmented<coord::phys_angle_t> &get_angles() const;

	/**
     * Set the angle at a given time.
     *
     * This adds a new keyframe to the angle curve.
     *
     * @param time Time at which the angle is set.
     * @param angle New angle.
     */
	void set_angle(const curve::time_t &time, const coord::phys_angle_t &angle);

private:
	/**
     * Position storage over time.
     */
	curve::Continuous<coord::phys3> position;

	/**
     * Angle the entity is facing over time.
     *
     * Represents degrees in the range [0, 360). At angle 0, the entity is facing
     * towards the camera (direction vector {x, y} = {-1, 1}).
     *
     * Rotation is clockwise, so at 90 degrees the entity is facing left.
     */
	curve::Segmented<coord::phys_angle_t> angle;
};

} // namespace openage::gamestate::component
