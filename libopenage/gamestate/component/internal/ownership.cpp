// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "ownership.h"
#include "../component_type.h"


namespace openage::gamestate::component {

component_t Ownership::get_component_type() const {
	return component_t::OWNERSHIP;
}

} // namespace openage::gamestate::component
