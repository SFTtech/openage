// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "position.h"

#include "gamestate/component/types.h"
#include "gamestate/definitions.h"


namespace openage::gamestate::component {
Position::Position(const std::shared_ptr<openage::event::EventLoop> &loop,
                   const coord::phys3 &initial_pos,
                   const curve::time_t &creation_time) :
	position(loop, 0, "", nullptr, WORLD_ORIGIN),
	angle(loop, 0) {
	this->position.set_insert(creation_time, initial_pos);

	// TODO: testing values
	this->position.set_insert(creation_time + 1, initial_pos + coord::phys3_delta{0, 1, 0});
	this->position.set_insert(creation_time + 2, initial_pos + coord::phys3_delta{1, 2, 0});
	this->position.set_insert(creation_time + 3, initial_pos + coord::phys3_delta{2, 2, 0});
	this->position.set_insert(creation_time + 4, initial_pos + coord::phys3_delta{3, 1, 0});
	this->position.set_insert(creation_time + 5, initial_pos + coord::phys3_delta{3, 0, 0});
	this->position.set_insert(creation_time + 6, initial_pos + coord::phys3_delta{2, -1, 0});
	this->position.set_insert(creation_time + 7, initial_pos + coord::phys3_delta{1, -1, 0});
	this->position.set_insert(creation_time + 8, initial_pos);

	this->angle.set_insert(creation_time, 270);
	this->angle.set_insert(creation_time + 1, 225);
	this->angle.set_insert(creation_time + 2, 180);
	this->angle.set_insert(creation_time + 3, 135);
	this->angle.set_insert(creation_time + 4, 90);
	this->angle.set_insert(creation_time + 5, 45);
	this->angle.set_insert(creation_time + 6, 0);
	this->angle.set_insert(creation_time + 7, 315);
	this->angle.set_insert(creation_time + 8, 270);
}


inline component_t Position::get_type() const {
	return component_t::POSITION;
}

const curve::Continuous<coord::phys3> &Position::get_positions() const {
	return this->position;
}

const curve::Continuous<angle_t> &Position::get_angles() const {
	return this->angle;
}

} // namespace openage::gamestate::component
