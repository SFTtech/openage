// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once

#include "../../../curve/discrete.h"
#include "../internal_component.h"

namespace openage::gamestate::component {

class Ownership : InternalComponent {
public:
	/**
     * ID of a game entity's owner at a given time.
     */
	curve::Discrete<uint64_t> owner;

	/**
	 * Creates an Ownership component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 * @param owner_id ID of the entity's owner.
	 * @param creation_time Ingame creation time of the component.
	 */
	Ownership(const std::shared_ptr<event::Loop> &loop,
	          const uint64_t owner_id,
	          const time_t &creation_time);

	component_t get_component_type() const override;
};

} // namespace openage::gamestate::component
