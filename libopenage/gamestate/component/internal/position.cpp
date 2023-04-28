// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "position.h"

#include "../component_type.h"


namespace openage::gamestate::component {
Position::Position(const std::shared_ptr<openage::event::Loop> &loop,
                   const coord::phys3 &initial_pos,
                   const curve::time_t &creation_time) :
	position(loop, 0) {
	this->position.set_insert(creation_time, initial_pos);

	// ASDF: testing
	this->position.set_insert(creation_time + 3, initial_pos + coord::phys3_delta{0, 2, 0});
	this->position.set_insert(creation_time + 6, initial_pos + coord::phys3_delta{2, 2, 0});
	this->position.set_insert(creation_time + 9, initial_pos + coord::phys3_delta{2, 0, 0});
	this->position.set_insert(creation_time + 12, initial_pos);
}


inline component_t Position::get_component_type() const {
	return component_t::POSITION;
}

const curve::Continuous<coord::phys3> &Position::get_position() const {
	return this->position;
}

} // namespace openage::gamestate::component
