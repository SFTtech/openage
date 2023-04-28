// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "position.h"

#include "../component_type.h"


namespace openage::gamestate::component {
Position::Position(const std::shared_ptr<openage::event::Loop> &loop,
                   const coord::phys3 &initial_pos,
                   const curve::time_t &creation_time) :
	position(loop, 0) {
	this->position.set_insert(creation_time, initial_pos);
}


inline component_t Position::get_component_type() const {
	return component_t::POSITION;
}

} // namespace openage::gamestate::component
