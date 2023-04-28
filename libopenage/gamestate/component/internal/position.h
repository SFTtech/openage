// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "coord/phys.h"
#include "curve/continuous.h"
#include "gamestate/component/internal_component.h"

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
	Position(const std::shared_ptr<openage::event::Loop> &loop,
	         const coord::phys3 &initial_pos,
	         const curve::time_t &creation_time);

	component_t get_component_type() const override;

private:
	/**
     * Entity position storage over time.
     */
	curve::Continuous<coord::phys3> position;
};

} // namespace openage::gamestate::component
