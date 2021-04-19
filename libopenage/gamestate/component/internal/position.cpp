// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "position.h"
#include "../component_type.h"


namespace openage::gamestate::component {

component_t Position::get_component_type() const {
	return component_t::POSITION;
}

} // namespace openage::gamestate::component
