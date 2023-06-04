// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "curve/discrete.h"
#include "gamestate/component/internal_component.h"

namespace openage::gamestate::component {

using ownership_id_t = uint64_t;

class Ownership : public InternalComponent {
public:
	/**
	 * Creates an Ownership component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 * @param owner_id Initial owner ID at creation time.
	 * @param creation_time Ingame creation time of the component.
	 */
	Ownership(const std::shared_ptr<openage::event::Loop> &loop,
	          const ownership_id_t owner_id,
	          const curve::time_t &creation_time);

	component_t get_component_type() const override;

private:
	/**
     * Owner ID storage over time.
     */
	curve::Discrete<ownership_id_t> owner;
};

} // namespace openage::gamestate::component
