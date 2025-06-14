// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "line_of_sight.h"


namespace openage::gamestate::component {

component_t LineOfSight::get_type() const {
	return component_t::LINE_OF_SIGHT;
}

} // namespace openage::gamestate::component
