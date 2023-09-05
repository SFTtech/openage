// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "idle.h"

#include "gamestate/component/types.h"

namespace openage::gamestate::component {

component_t Idle::get_type() const {
	return component_t::IDLE;
}

} // namespace openage::gamestate::component
