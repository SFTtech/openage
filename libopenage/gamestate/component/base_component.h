// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once

#include "component_type.h"

namespace openage::gamestate::component {

/**
 * Interface for components.
 */
class Component {
public:
	virtual ~Component() = default;

	/**
     * Get the component type of the component.
     *
     * @return Component type of the component.
     */
	virtual component_t get_component_type() const = 0;
};

} // namespace openage::gamestate::component
