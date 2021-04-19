// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "idle.h"
#include "../component_type.h"

namespace openage::gamestate::component {

component_t Idle::get_component_type() const {
	return component_t::IDLE;
}

} // namespace openage::gamestate::component
