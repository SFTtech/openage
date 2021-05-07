// Copyright 2021-2021 the openage authors. See copying.md for legal info.


#include "move.h"
#include "../component_type.h"


namespace openage::gamestate::component {

inline component_t Move::get_component_type() const {
	return component_t::TURN;
}

} // namespace openage::gamestate::component
