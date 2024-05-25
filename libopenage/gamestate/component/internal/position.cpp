// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#include "position.h"

#include "gamestate/component/types.h"
#include "gamestate/definitions.h"
#include "util/fixed_point.h"


namespace openage::gamestate::component {
Position::Position(const std::shared_ptr<openage::event::EventLoop> &loop,
                   const coord::phys3 &initial_pos,
                   const time::time_t &creation_time) :
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

	this->angle.set_insert(creation_time, coord::phys_angle_t::from_int(315));
	this->angle.set_insert_jump(creation_time + 1, coord::phys_angle_t::from_int(315), coord::phys_angle_t::from_int(270));
	this->angle.set_insert_jump(creation_time + 2, coord::phys_angle_t::from_int(270), coord::phys_angle_t::from_int(225));
	this->angle.set_insert_jump(creation_time + 3, coord::phys_angle_t::from_int(225), coord::phys_angle_t::from_int(180));
	this->angle.set_insert_jump(creation_time + 4, coord::phys_angle_t::from_int(180), coord::phys_angle_t::from_int(135));
	this->angle.set_insert_jump(creation_time + 5, coord::phys_angle_t::from_int(135), coord::phys_angle_t::from_int(90));
	this->angle.set_insert_jump(creation_time + 6, coord::phys_angle_t::from_int(90), coord::phys_angle_t::from_int(45));
	this->angle.set_insert_jump(creation_time + 7, coord::phys_angle_t::from_int(45), coord::phys_angle_t::from_int(0));
	this->angle.set_insert_jump(creation_time + 8, coord::phys_angle_t::from_int(0), coord::phys_angle_t::from_int(315));
}


Position::Position(const std::shared_ptr<openage::event::EventLoop> &loop) :
	position(loop, 0, "", nullptr, WORLD_ORIGIN),
	angle(loop, 0) {
}

inline component_t Position::get_type() const {
	return component_t::POSITION;
}

const curve::Continuous<coord::phys3> &Position::get_positions() const {
	return this->position;
}

void Position::set_position(const time::time_t &time, const coord::phys3 &pos) {
	this->position.set_last(time, pos);
}

const curve::Segmented<coord::phys_angle_t> &Position::get_angles() const {
	return this->angle;
}

void Position::set_angle(const time::time_t &time, const coord::phys_angle_t &angle) {
	auto old_angle = this->angle.get(time);
	this->angle.set_last_jump(time, old_angle, angle);
}

} // namespace openage::gamestate::component
