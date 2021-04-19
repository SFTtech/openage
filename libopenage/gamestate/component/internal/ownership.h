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

	component_t get_component_type() const override;
};

} // namespace openage::gamestate::component
