// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once

#include "component_type.h"

namespace openage::gamestate::component {

/**
 * Superclass for components.
 */
class Component {
public:
	/**
     * Get the component type of the component.
     *
     * @return Component type of the component.
     */
	virtual const component_t get_component_type() const;
};

} // namespace openage::gamestate::component
