// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include <cmath>

#include "../pathfinding/a_star.h"
#include "../pathfinding/heuristics.h"
#include "../terrain/terrain.h"
#include "../util/unique.h"
#include "action.h"
#include "command.h"
#include "producer.h"
#include "unit.h"
#include "unit_texture.h"

namespace openage {

bool UnitAction::show_debug = false;

UnitAction::UnitAction(Unit *u, graphic_type gt)
	:
	entity{u},
	graphic{gt},
	frame{.0f},
	frame_rate{.0f} {

	if (u->graphics->count(gt) > 0) {
		this->frame_rate = u->graphics->at(gt)->frame_rate;
	}

	if (frame_rate == 0) {
		// TODO: this breaks the trees
		//frame = rand(); //randomize variation graphics
	}
}

graphic_type UnitAction::type() const {
	return this->graphic;
}

float UnitAction::current_frame() const {
	return this->frame;
}

void UnitAction::draw_debug() {
	// draw debug content if available
	if(show_debug && this->debug_draw_action) this->debug_draw_action();
}

void UnitAction::face_towards(const coord::phys3 pos) {
	if (this->entity->has_attribute(attr_type::direction)) {
		auto &d_attr = this->entity->get_attribute<attr_type::direction>();
		d_attr.unit_dir = pos - this->entity->location->pos.draw;
	}
}

void UnitAction::damage_object(Unit *target, unsigned dmg) {
	if (target->has_attribute(attr_type::hitpoints)) {
		auto &hp = target->get_attribute<attr_type::hitpoints>();
		hp.current -= dmg;
	}
}

DeadAction::DeadAction(Unit *e, std::function<void()> on_complete)
	:
	UnitAction(e, graphic_type::dying),
	end_frame{.0f},
	on_complete_func{on_complete} {

	if (this->entity->graphics->count(graphic) > 0) {
		this->end_frame = this->entity->graphics->at(graphic)->frame_count - 1;
	}
}

void DeadAction::update(unsigned int time) {
	if (this->entity->has_attribute(attr_type::hitpoints)) {
		auto &h_attr = this->entity->get_attribute<attr_type::hitpoints>();
		h_attr.current = 0;
	}

	// inc frame but do not pass the end frame
	if (this->frame < this->end_frame) {
		this->frame += 0.01 + time * this->frame_rate / 3.0f;
	}
	else {
		this->frame = this->end_frame;
	}
	

	if (this->completed()) {
		this->on_complete_func();
	}
}

bool DeadAction::completed() {
	// check resource, trees/huntables with resource are not removed
	if (this->entity->has_attribute(attr_type::resource)) {
		auto &res_attr = this->entity->get_attribute<attr_type::resource>();
		if (res_attr.amount > 0) {
			return false; // cannot complete when resource remains
		}
	}
	return this->frame > this->end_frame;
}

void IdleAction::update(unsigned int time) {

	// possibly use a seperate action for building construction
	if (this->entity->has_attribute(attr_type::building)) {
		auto &build = this->entity->get_attribute<attr_type::building>();
		if (build.completed < 1.0f) {
			this->graphic = graphic_type::construct;
		}
		else {
			this->graphic = graphic_type::standing;
		}
	}

	// inc frame
	this->frame += time * this->frame_rate / 20.0f;
}

bool IdleAction::completed() {
	if (this->entity->has_attribute(attr_type::hitpoints)) {
		auto &hp = this->entity->get_attribute<attr_type::hitpoints>();
		if (hp.current <= 0) {
			log::dbg("unit has 0 hitpoints");
		}
		return hp.current <= 0;
	}
	return false;
}

MoveAction::MoveAction(Unit *e, coord::phys3 tar, bool repath)
	:
	UnitAction{e, graphic_type::walking},
	unit_target{},
	target(tar),
	radius{path::path_grid_size},
	allow_repath{repath} {

	// switch workers to the carrying graphic
	if (this->entity->has_attribute(attr_type::gatherer)) {
		auto &gather_attr = this->entity->get_attribute<attr_type::gatherer>();
		if (gather_attr.amount > 0) {
			this->graphic = graphic_type::carrying;
		}
	}

	// set an initial path
	this->set_path();
	this->debug_draw_action = [&]() {
		this->path.draw_path();
	};
}

MoveAction::MoveAction(Unit *e, UnitReference tar, coord::phys_t rad)
	:
	UnitAction{e, graphic_type::walking},
	unit_target{tar},
	target(tar.get()->location->pos.draw),
	radius{rad},
	allow_repath{false} {

	// switch workers to the carrying graphic
	if (this->entity->has_attribute(attr_type::gatherer)) {
		auto &gather_attr = this->entity->get_attribute<attr_type::gatherer>();
		if (gather_attr.amount > 0) {
			this->graphic = graphic_type::carrying;
		}
	}

	// set an initial path
	this->set_path();
	this->debug_draw_action = [&]() {
		this->path.draw_path();
	};
}

MoveAction::~MoveAction() {}

void MoveAction::update(unsigned int time) {
	if (this->unit_target.is_valid()) {
		// a unit is targeted, which may move
		coord::phys3 &target_pos = this->unit_target.get()->location->pos.draw;
		coord::phys3 &unit_pos = this->entity->location->pos.draw;

		// repath if target changes tiles by a threshold
		// this repathing is more frequent when the unit is
		// close to its target
		coord::phys_t tdx = target_pos.ne - this->target.ne;
		coord::phys_t tdy = target_pos.se - this->target.se;
		coord::phys_t udx = unit_pos.ne - this->target.ne;
		coord::phys_t udy = unit_pos.se - this->target.se;
		if (this->path.waypoints.empty() || std::hypot(tdx, tdy) > std::hypot(udx, udy)) {
			this->target = target_pos;
			this->set_path();
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
			move_dir = (move_dir * distance_to_move) / distance_to_waypoint;

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
	else {
		// cases for modifying path when blocked
		if (this->allow_repath) {
			log::dbg("path blocked -- finding new path");
			this->set_path();
		}
		else {
			log::dbg("path blocked -- drop action");
			this->path.waypoints.clear();
		}
	}

	// inc frame
	this->frame += time * this->frame_rate / 5.0f;
}

bool MoveAction::completed() {
	if (this->unit_target.is_valid()) {
		// distance from the units edge
		coord::phys3 &unit_loc = this->entity->location->pos.draw;
		this->distance_to_target = this->unit_target.get()->location->from_edge(unit_loc);
	}
	else {
		// no more waypoints to a static location
		if (this->path.waypoints.empty()) {
			return true;
		}
		coord::phys3_delta move_dir = target - this->entity->location->pos.draw;

		// normalise dir
		this->distance_to_target = (coord::phys_t) std::hypot(move_dir.ne, move_dir.se);
	}

	/*
	 * close enough to end action
	 */
	if (this->distance_to_target < this->radius) {
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
	if (this->unit_target.is_valid()) {
		this->path = path::to_object(this->entity->location, this->unit_target.get()->location, this->radius);
	}
	else {
		coord::phys3 start = this->entity->location->pos.draw;
		coord::phys3 end = this->target;
		this->path = path::to_point(start, end, this->entity->location->passable);
	}
}

TrainAction::TrainAction(Unit *e, UnitProducer *pp)
	:
	UnitAction{e, graphic_type::standing},
	trained{pp},
	complete{false} {
}

void TrainAction::update(unsigned int) {
	UnitContainer *container = this->entity->get_container();
	Terrain *terrain = this->entity->location->get_terrain();

	// find a free position adjacent to the building
	coord::phys3 p = this->entity->location->pos.draw;
	coord::tile t = p.to_tile3().to_tile() + coord::tile_delta{2, 2};
	for (int i = 1; i < 6; ++i) {
		if (terrain->get_data(t)->obj.empty()) {
			break;
		}
		t.ne -= 1;
	}
	coord::phys3 pos = t.to_phys2().to_phys3();


	// create using the producer
	auto uref = container->new_unit(*this->trained, terrain, pos);
	if (uref.is_valid()) {

		// use a move command with the position
		// TODO: use a position on edge of the buildings
		Command cmd(coord::tile{ 8, 10 }.to_phys2().to_phys3());
		cmd.set_ability(ability_type::move);
		uref.get()->invoke(cmd);

		// try again next update if cannot place
		this->complete = true;
	}
}

BuildAction::BuildAction(Unit *e, UnitProducer *pp, const coord::phys3 &pos)
	:
	UnitAction{e, graphic_type::gather},
	building{nullptr},
	producer{pp},
	position(pos),
	complete{.0f},
	range{path::path_grid_size + (e->location->min_axis() / 2)} {
}

BuildAction::BuildAction(Unit *e, Unit *foundation)
	:
	UnitAction{e, graphic_type::gather},
	building{foundation},
	producer{nullptr},
	complete{.0f},
	range{path::path_grid_size + (e->location->min_axis() / 2)} {
}

void BuildAction::update(unsigned int time) {
	if (this->building) {

		// the worker attributes attached to the unit
		auto &gatherer_attr = this->entity->get_attribute<attr_type::gatherer>();

		// set relevant graphics
		this->entity->graphics = &gatherer_attr.graphics[gamedata::unit_classes::BUILDING]->graphics;

		// set direction unit should face
		TerrainObject *target_location = this->building->location;
		this->face_towards(target_location->pos.draw);

		// move to resource being collected
		auto distance_to_dropsite = target_location->from_edge(this->entity->location->pos.draw);
		if (distance_to_dropsite > this->range) {

			// move to dropsite
			Command cmd(this->building);
			cmd.set_ability(ability_type::move);
			this->entity->invoke(cmd);
			return;
		}
		else {
			// increment building completion
			auto &build = this->building->get_attribute<attr_type::building>();
			build.completed += 0.001;
			this->complete = build.completed;
		}
	}
	else {

		// create foundation using the producer
		UnitContainer *container = this->entity->get_container();
		Terrain *terrain = this->entity->location->get_terrain();
		auto uref = container->new_unit(*this->producer, terrain, this->position);
		if (uref.is_valid()) {
			this->building = uref.get();
		}
	}

	// inc frame
	this->frame += time * this->frame_rate / 3.0f;
}

GatherAction::GatherAction(Unit *e, UnitReference tar)
	:
	UnitAction{e, graphic_type::gather},
	target{tar},
	dropsite{},
	range{path::path_grid_size + (e->location->min_axis() / 2)} {

	// find nearest dropsite from the targeted resource
	TerrainObject *ds = path::find_nearest(this->target.get()->location,
		[=](const openage::TerrainObject *other) -> bool {
			if (!other || !other->unit) {
				return false;
			}
			return other->unit != this->entity &&
			       other->unit != tar.get() &&
			       other->unit->has_attribute(attr_type::building);
	});

	if (ds && ds->unit) {
		log::dbg("dropsite found");
		this->dropsite = ds->unit->get_ref();
	}
	else {
		log::dbg("no dropsite found");
	}
}

GatherAction::~GatherAction() {}

void GatherAction::update(unsigned int time) {
	if (!this->target.is_valid() || !this->dropsite.is_valid()) return;

	// the targets attributes
	auto &resource_attr = this->target.get()->get_attribute<attr_type::resource>();

	// the gatherer attributes attached to the unit
	auto &gatherer_attr = this->entity->get_attribute<attr_type::gatherer>();
	gatherer_attr.current_type = resource_attr.resource_type;

	// set relevant graphics
	this->entity->graphics = &gatherer_attr.graphics[this->target.get()->unit_class]->graphics;

	// set direction unit should face
	TerrainObject *target_location = this->target.get()->location;
	this->face_towards(target_location->pos.draw);

	// return to dropsite
	if (gatherer_attr.amount > gatherer_attr.capacity) {
		// dropsite position
		TerrainObject *dropsite_location = this->dropsite.get()->location;

		// move to resource being collected
		auto distance_to_dropsite = dropsite_location->from_edge(this->entity->location->pos.draw);
		if (distance_to_dropsite > this->range) {

			// move to dropsite
			Command cmd(this->dropsite.get());
			cmd.set_ability(ability_type::move);
			this->entity->invoke(cmd);
			return;
		}
		else {
			// TODO: add value to player stockpile
			gatherer_attr.amount = 0.0f;
		}
	}

	// attack objects which have hitpoints (trees, hunt, sheep)
	if (this->target.get()->has_attribute(attr_type::hitpoints)) {
		auto &hp_attr = this->target.get()->get_attribute<attr_type::hitpoints>();
		if (hp_attr.current > 0) {
			Command cmd(this->target.get());
			cmd.set_ability(ability_type::attack);
			this->entity->invoke(cmd);
			return;
		}
	}

	// move to resource being collected
	this->distance_to_target = target_location->from_edge(this->entity->location->pos.draw);
	if (distance_to_target > range) {
		Command cmd(this->target.get());
		cmd.set_ability(ability_type::move);
		this->entity->invoke(cmd);
		return;
	}

	// transfer using gather rate
	gatherer_attr.amount += gatherer_attr.gather_rate * time;
	resource_attr.amount -= gatherer_attr.gather_rate * time;

	// inc frame
	this->frame += time * this->frame_rate / 3.0f;
}

bool GatherAction::completed() {
	if (!this->target.is_valid() || !this->dropsite.is_valid()) return true;
	return false;
}

AttackAction::AttackAction(Unit *e, UnitReference tar)
	:
	UnitAction{e, graphic_type::attack},
	target{tar},
	range{3*path::path_grid_size + (e->location->min_axis() / 2)},
	strike_percent{0.0f},
	rate_of_fire{0.002f} {
}

AttackAction::~AttackAction() {}

void AttackAction::update(unsigned int time) {
	if (!this->target.is_valid()) return;

	// set direction unit should face
	this->face_towards(this->target.get()->location->pos.draw);

	// find distance
	this->distance_to_target = target.get()->location->from_edge(this->entity->location->pos.draw);

	// subtract range of this unit
	auto &attack = this->entity->get_attribute<attr_type::attack>();
	this->distance_to_target -= attack.range;

	if (strike_percent > 0.0) {
		strike_percent -= rate_of_fire * time;

		// inc frame
		this->frame += time * this->entity->graphics->at(graphic)->frame_count * rate_of_fire;
	}
	else if (distance_to_target <= range) {
		strike_percent = 1.0f;
		this->frame = 0.0f;
		this->attack(this->target.get());
	}
	else {
		// out of range so try move towards
		// if this unit has a move ability
		Command cmd(this->target.get());
		cmd.set_ability(ability_type::move);
		this->entity->invoke(cmd);
	}
}

bool AttackAction::completed() {
	if (!this->target.is_valid()) return true;
	auto &h_attr = this->target.get()->get_attribute<attr_type::hitpoints>();
	return h_attr.current < 1; // is unit still alive?
}

void AttackAction::attack(Unit *target) {
	auto &attack = this->entity->get_attribute<attr_type::attack>();
	if (attack.pp) {

		// add projectile to the game
		this->fire_projectile(attack, target->location->pos.draw);
	}
	else {
		this->damage_object(target, 1);
	}
}

void AttackAction::fire_projectile(const Attribute<attr_type::attack> &att, const coord::phys3 &target) {
	if (!this->target.is_valid()) {
		return;
	}

	// container terrain and initial position
	UnitContainer *container = this->entity->get_container();
	Terrain *terrain = this->entity->location->get_terrain();
	coord::phys3 current_pos = this->entity->location->pos.draw;
	current_pos.up = att.init_height;

	// create using the producer
	auto projectile_ref = container->new_unit(*att.pp, terrain, current_pos);

	// send towards target using a projectile ability (creates projectile motion action)
	if (projectile_ref.is_valid()) {
		auto projectile = projectile_ref.get();
		auto &projectile_attr = projectile->get_attribute<attr_type::projectile>();
		projectile_attr.launcher = this->entity->get_ref();
		projectile_attr.launched = true;
		projectile->push_action(util::make_unique<ProjectileAction>(projectile, target), true);
	}
	else {
		log::dbg("projectile launch failed");
	}
}

ProjectileAction::ProjectileAction(Unit *e, coord::phys3 target)
	:
	UnitAction{e, graphic_type::standing},
	has_hit{false} {

	// find speed to move
	auto &sp_attr = this->entity->get_attribute<attr_type::speed>();
	coord::phys_t projectile_speed = sp_attr.unit_speed;

	// arc of projectile
	auto &pr_attr = this->entity->get_attribute<attr_type::projectile>();
	float projectile_arc = pr_attr.projectile_arc;

	// distance and time to target
	coord::phys3_delta d = target - this->entity->location->pos.draw;
	coord::phys_t distance_to_target = (coord::phys_t) std::hypot(d.ne, d.se);
	int flight_time = distance_to_target / projectile_speed;


	if (projectile_arc < 0) {
		// TODO negative values probably indicate something
		projectile_arc += 0.2;
	}

	// now figure gravity from arc parameter
	// TODO projectile arc is the ratio between horizontal and
	// vertical components of the initial direction

	this->grav = 0.01f * (exp(pow(projectile_arc, 0.5f)) - 1) * projectile_speed;
	log::dbg("gravity = %ld", this->grav);

	// inital launch direction
	auto &d_attr = this->entity->get_attribute<attr_type::direction>();
	d_attr.unit_dir = (projectile_speed * d) / distance_to_target;

	// account for initial height
	coord::phys_t initial_height = this->entity->location->pos.draw.up;
	d_attr.unit_dir.up = (grav * flight_time) / 2 - (initial_height / flight_time);
}

ProjectileAction::~ProjectileAction() {}

void ProjectileAction::update(unsigned int time) {
	auto &d_attr = this->entity->get_attribute<attr_type::direction>();

	// apply gravity
	d_attr.unit_dir.up -= this->grav * time;

	coord::phys3 new_position = this->entity->location->pos.draw + d_attr.unit_dir * time;
	if (!this->entity->location->move(new_position)) {
		log::dbg("arrow hit object");

		// find object which was hit
		Terrain *terrain = this->entity->location->get_terrain();
		TileContent *tc = terrain->get_data(new_position.to_tile3().to_tile());
		if (tc && !tc->obj.empty()) {
			for (auto loc : tc->obj) {
				if (this->entity->location != loc) {
					this->damage_object(loc->unit, 1);
					break;
				}
			}
		}

		has_hit = true;
	}

	// inc frame
	this->frame += time * this->frame_rate;
}

bool ProjectileAction::completed() {
	return (has_hit || this->entity->location->pos.draw.up <= 0);
}

} /* namespace openage */
