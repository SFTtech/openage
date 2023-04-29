// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "turn.h"

#include "gamestate/component/types.h"


namespace openage::gamestate::component {

component_t Turn::get_type() const {
	return component_t::TURN;
}

} // namespace openage::gamestate::component
