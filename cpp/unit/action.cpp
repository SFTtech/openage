// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include <cmath>

#include "../game_main.h"
#include "../pathfinding/a_star.h"
#include "../pathfinding/heuristics.h"
#include "action.h"
#include "unit.h"

namespace openage {

UnitAction::UnitAction(Unit *u, Texture *t, TestSound *s, float fr)
	:
	entity(u),
	tex(t),
	on_begin(s),
	frame(.0f),
	frame_rate(fr) {
}

void UnitAction::draw() {
	// play sound if available
	if (this->on_begin && this->frame == 0.0f) {
		on_begin->play();
	}

	coord::phys3 &draw_pos = this->entity->location->pos.draw;
	unsigned color = 0;
	if (this->entity->has_attribute(attr_type::color)) {
		auto &c_attr = this->entity->get_attribute<attr_type::color>();
		color = c_attr.color;
	}
	if (this->tex->get_subtexture_count() < 5
	    || !this->entity->has_attribute(attr_type::direction)) {

		// draw single frame
		this->tex->draw(draw_pos.to_camgame(), PLAYERCOLORED, false, 0, color);
		this->frame += this->frame_rate; // sound gets played on frame 0
		return;
	}

	/*
	 * assume group has 5 image sets
	 * mirroring is used to make additional image sets
	 */
	auto &d_attr = this->entity->get_attribute<attr_type::direction>();
	coord::phys3_delta draw_dir = d_attr.unit_dir;
	bool mirror = false;
	uint angle = dir_group(draw_dir);
	if (angle > 4) {
		angle = 8 - angle;
		mirror = true;
	}

	uint groupsize = this->tex->get_subtexture_count() / 5;
	uint to_draw = angle * groupsize + ((uint) frame % groupsize);
	this->tex->draw(draw_pos.to_camgame(), PLAYERCOLORED, mirror, to_draw, color);
	this->frame += this->frame_rate;
}

void DeadAction::update(unsigned int) {
	if ( this->entity->has_attribute(attr_type::hitpoints) ) {
		auto &h_attr = this->entity->get_attribute<attr_type::hitpoints>();
		h_attr.current = 0;
	}
}

bool DeadAction::completed() {
	return this->frame > (this->tex->get_subtexture_count() / 5);
}

MoveAction::MoveAction(Unit *e, Texture *t, TestSound *s, coord::phys3 tar)
	:
	UnitAction(e, t, s),
	unit_target(nullptr),
	target(tar),
	radius(7500) {

	// set an initial path
	this->set_path();
}

MoveAction::MoveAction(Unit *e, Texture *t, TestSound *s, Unit *tar, coord::phys_t rad)
	:
	UnitAction(e, t, s),
	unit_target(tar),
	target(tar->location->pos.draw),
	radius(rad) {

	// set an initial path
	this->set_path();
}

MoveAction::~MoveAction() {}

void MoveAction::update(unsigned int time) {
	bool repath = false;
	if (this->unit_target) {
		coord::phys3 old_target = this->target;
		this->target = this->unit_target->location->pos.draw;

		// repath if target changes tiles by a threshold
		coord::phys_t dx = old_target.ne - this->target.ne;
		coord::phys_t dy = old_target.se - this->target.se;
		if (std::hypot(dx, dy) > (1 << 16)) {
			repath = true;
		}

		if ( this->unit_target->has_attribute(attr_type::hitpoints) &&
		     this->unit_target->get_attribute<attr_type::hitpoints>().current < 1) {
			this->unit_target = nullptr;
		}
	}
	if (this->path.waypoints.empty()) {
		return;
	}

	// find distance to move in this update
	auto &sp_attr = this->entity->get_attribute<attr_type::speed>();
	coord::phys_t distance_to_move = sp_attr.unit_speed * time;

	// current position and direction
	coord::phys3 new_position = this->entity->location->pos.draw;
	auto &d_attr = this->entity->get_attribute<attr_type::direction>();
	coord::phys3_delta new_direction = d_attr.unit_dir;

	while (distance_to_move > 0) {
		if (this->path.waypoints.empty()) {
			break;
		}

		// find a point to move directly towards
		coord::phys3 waypoint = this->next_waypoint();
		coord::phys3_delta move_dir = waypoint - new_position;

		// normalise dir
		coord::phys_t distance_to_waypoint = (coord::phys_t) std::hypot(move_dir.ne, move_dir.se);

		if (distance_to_waypoint <= distance_to_move) {
			distance_to_move -= distance_to_waypoint;

			// change entity position and direction
			new_position = waypoint;
			new_direction = move_dir;

			// remove the waypoint
			this->path.waypoints.pop_back();
		}
		else {
			// distance_to_waypoint is larger so need to divide
			move_dir /= (distance_to_waypoint / distance_to_move);

			// change entity position and direction
			new_position += move_dir;
			new_direction = move_dir;
			break;
		}
	}
	
	// check move collisions
	bool move_completed = this->entity->location->move(new_position);
	if (move_completed) {
		d_attr.unit_dir = new_direction;
	}

	// cases for modifying path
	if (this->unit_target && !move_completed) {
		log::dbg("path blocked -- drop action");
		this->path.waypoints.clear();
	}
	else if (repath || !move_completed) {
		// move failed -- try creating a new path
		log::dbg("path blocked -- finding new path");
		this->set_path();
	}
}

bool MoveAction::completed() {
	if (this->path.waypoints.empty()) {
		return true;
	}

	coord::phys3_delta move_dir = target - this->entity->location->pos.draw;

	// normalise dir
	this->distance_to_target = (coord::phys_t) std::hypot(move_dir.ne, move_dir.se);

	/*
	 * close enough to end action
	 */
	if (this->distance_to_target < radius) {
		return true;
	}
	return false;
}

coord::phys3 MoveAction::next_waypoint() const {
	if (this->path.waypoints.size() > 0) {
		return this->path.waypoints.back().position;
	} else {
		throw util::Error{"no next waypoint available!"};
	}
}

void MoveAction::set_path() {
	if (this->unit_target) {
		this->path = path::to_object(this->entity->location, this->unit_target->location);
	}
	else {
		coord::phys3 start = this->entity->location->pos.draw;
		coord::phys3 end = this->target;
		this->path = path::to_point(start, end, this->entity->location->passable);
	}
}

GatherAction::GatherAction(Unit *e, Unit *tar, Texture *t, TestSound *s)
	:
	UnitAction( e, t, s ),
	target(tar),
	range(40000),
	carrying(0) {
}

GatherAction::~GatherAction() {}

void GatherAction::update(unsigned int time) {
	if (carrying > 10) {
		auto move_ability = this->entity->get_ability(ability_type::move);
		this->entity->push_action(move_ability->target(this->entity, coord::phys3{0, 0, 0}));
		carrying = 0;
	}

	// set direction unit should face
	if (this->entity->has_attribute(attr_type::direction)) {
		auto &d_attr = this->entity->get_attribute<attr_type::direction>();
		d_attr.unit_dir = this->target->location->pos.draw - this->entity->location->pos.draw;
	}

	this->distance_to_target = target->location->from_edge(this->entity->location->pos.draw);
	if (distance_to_target > range) {
		auto move_ability = this->entity->get_ability(ability_type::move);
		this->entity->push_action(move_ability->target(this->entity, target));
	}

	// gather rate
	carrying += 0.01 * time;
}

bool GatherAction::completed() {
	return false;
}

AttackAction::AttackAction(Unit *e, Unit *tar, Texture *t, TestSound *s)
	:
	UnitAction( e, t, s ),
	target(tar),
	range(40000),
	strike_percent(0.0f) {
}

AttackAction::~AttackAction() {}

void AttackAction::update(unsigned int time) {
	// set direction unit should face
	if (this->entity->has_attribute(attr_type::direction)) {
		auto &d_attr = this->entity->get_attribute<attr_type::direction>();
		d_attr.unit_dir = this->target->location->pos.draw - this->entity->location->pos.draw;
	}

	this->distance_to_target = target->location->from_edge(this->entity->location->pos.draw);
	if (strike_percent > 0.0) {
		strike_percent -= 0.002 * time;
		return;
	}
	else if (distance_to_target <= range) {
		strike_percent = 1.0f;
	}
	else {
		auto move_ability = this->entity->get_ability(ability_type::move);

		// if this unit can move
		if (move_ability) {
			this->entity->push_action(move_ability->target(this->entity, target));
		}
	}
}

bool AttackAction::completed() {
	auto &h_attr = this->target->get_attribute<attr_type::hitpoints>();
	return h_attr.current < 1; // is unit still alive?
}

} /* namespace openage */
