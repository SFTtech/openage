// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include <algorithm>
#include <cmath>

#include "../pathfinding/a_star.h"
#include "../pathfinding/heuristics.h"
#include "../terrain/terrain.h"
#include "../terrain/terrain_search.h"
#include "action.h"
#include "command.h"
#include "producer.h"
#include "unit_texture.h"

namespace openage {

bool UnitAction::show_debug = false;

coord::phys_t UnitAction::adjacent_range(Unit *u) {
	return 3 * path::path_grid_size + (u->location->min_axis() / 2);
}

coord::phys_t UnitAction::get_attack_range(Unit *u) {
	coord::phys_t range = adjacent_range(u);
	if (u->has_attribute(attr_type::attack)) {
		auto &attack = u->get_attribute<attr_type::attack>();
		range += attack.range;
	}
	return range;
}

coord::phys_t UnitAction::get_heal_range(Unit *u) {
	coord::phys_t range = adjacent_range(u);
	if (u->has_attribute(attr_type::heal)) {
		auto &heal = u->get_attribute<attr_type::heal>();
		range += heal.range;
	}
	return range;
}

UnitAction::UnitAction(Unit *u, graphic_type initial_gt)
	:
	entity{u},
	graphic{initial_gt},
	frame{.0f},
	frame_rate{.0f} {

	auto &g_set = this->current_graphics();
	if (g_set.count(initial_gt) > 0) {
		auto utex = g_set.at(initial_gt);
		if (utex) {
			this->frame_rate = utex->frame_rate;
		}
		else {
			this->entity->log(MSG(dbg) << "Broken graphic (null)");
		}
	}
	else {
		this->entity->log(MSG(dbg) << "Broken graphic (not available)");
	}

	if (this->frame_rate == 0) {

		// a random starting point for static graphics
		// this creates variations in trees / houses etc
		// this value is also deterministic to match across clients
		this->frame = (u->id * u->id * 19249) & 0xff;
	}
}

graphic_type UnitAction::type() const {
	return this->graphic;
}

float UnitAction::current_frame() const {
	return this->frame;
}

const graphic_set &UnitAction::current_graphics() const {

	// return the default graphic
	return this->entity->unit_type->graphics;
}

void UnitAction::draw_debug() {
	// draw debug content if available
	if (show_debug && this->debug_draw_action) {
		this->debug_draw_action();
	}
}

void UnitAction::face_towards(const coord::phys3 pos) {
	if (this->entity->has_attribute(attr_type::direction)) {
		auto &d_attr = this->entity->get_attribute<attr_type::direction>();
		d_attr.unit_dir = pos - this->entity->location->pos.draw;
	}
}

// TODO remove (keep for testing)
void UnitAction::damage_object(Unit &target, unsigned dmg) {
	if (target.has_attribute(attr_type::damaged)) {
		auto &dm = target.get_attribute<attr_type::damaged>();
		if (dm.hp > dmg) {
			dm.hp -= dmg;
		}
		else {
			dm.hp = 0;
		}
	}
}

void UnitAction::damage_object(Unit &target) {
	if (target.has_attribute(attr_type::damaged)) {
		auto &dm = target.get_attribute<attr_type::damaged>();

		if (target.has_attribute(attr_type::armor) && this->entity->has_attribute(attr_type::attack)) {
			auto &armor = target.get_attribute<attr_type::armor>().armor;
			auto &damage = this->entity->get_attribute<attr_type::attack>().damage;

			unsigned int actual_damage = 0;
			for (const auto &pair : armor) {
				auto search = damage.find(pair.first);
				if (search != damage.end()) {
					if (pair.second < search->second) {
						actual_damage += search->second - pair.second;
					}
				}
			}
			// TODO add elevation modifier here
			if (actual_damage < 1) {
				actual_damage = 1;
			}

			if (dm.hp > actual_damage) {
				dm.hp -= actual_damage;
			}
			else {
				dm.hp = 0;
			}
		}
		else {
			// TODO remove (keep for testing)
			damage_object(target, 1);
		}
	}
}

void UnitAction::move_to(Unit &target, bool use_range) {
	auto &player = this->entity->get_attribute<attr_type::owner>().player;
	Command cmd(player, &target);
	cmd.set_ability(ability_type::move);
	if (use_range) {
		cmd.add_flag(command_flag::use_range);
	}
	this->entity->queue_cmd(cmd);
}

TargetAction::TargetAction(Unit *u, graphic_type gt, UnitReference r, coord::phys_t rad)
	:
	UnitAction(u, gt),
	target{r},
	target_type_id{0},
	repath_attempts{10},
	end_action{false},
	radius{rad} {

	// update type
	if (this->target.is_valid()) {
		auto target_ptr = this->target.get();
		this->target_type_id = target_ptr->unit_type->id();
	}

	// initial value for distance
	this->update_distance();
}

TargetAction::TargetAction(Unit *u, graphic_type gt, UnitReference r)
	:
	TargetAction(u, gt, r, adjacent_range(u)) {
}

void TargetAction::update(unsigned int time) {
	auto target_ptr = update_distance();
	if (!target_ptr) {
		return; // target has become invalid
	}

	// this update moves a unit within radius of the target
	// once within the radius the update gets passed to the class
	// derived from TargetAction

	// set direction unit should face
	this->face_towards(target_ptr->location->pos.draw);

	// move to within the set radius
	if (this->dist_to_target <= this->radius) {

		// the derived class controls what to
		// do when in range of the target
		this->update_in_range(time, target_ptr);
		this->repath_attempts = 10;
	}
	else if (this->repath_attempts) {

		// out of range so try move towards
		// if this unit has a move ability
		this->move_to(*target_ptr);
		this->repath_attempts -= 1;
	}
	else {

		// unit is stuck
		this->end_action = true;
	}
}

void TargetAction::on_completion() {
	if (!this->entity->location) {
		return;
	}

	// TODO: retask units on nearby objects
	// such as gathers targeting a new resource
	// when the current target expires

	// find a different target with same type
	TerrainObject *new_target = nullptr;
	if (this->name() == "gather") {
		new_target = find_near(*this->entity->location,
			[this](const TerrainObject &obj) {
				return obj.unit.unit_type->id() == this->target_type_id &&
				       !obj.unit.has_attribute(attr_type::worker) &&
				       obj.unit.has_attribute(attr_type::resource) &&
				       obj.unit.get_attribute<attr_type::resource>().amount > 0.0;
			});
	}

	if (new_target) {
		this->entity->log(MSG(dbg) << "auto retasking");
		auto &pl_attr = this->entity->get_attribute<attr_type::owner>();
		Command cmd(pl_attr.player, &new_target->unit);
		this->entity->queue_cmd(cmd);
	}
}

bool TargetAction::completed() const {
	if (this->end_action ||
	    !this->target.is_valid() ||
	    !this->target.get()->location) {
		return true;
	}
	return this->completed_in_range(this->target.get());
}

coord::phys_t TargetAction::distance_to_target() {
	return this->dist_to_target;
}

Unit *TargetAction::update_distance() {
	if (!this->target.is_valid()) {
		return nullptr;
	}

	// make sure object is not garrisoned
	auto target_ptr = this->target.get();
	if (!target_ptr->location) {
		return nullptr;
	}

	// update distance
	this->dist_to_target = target_ptr->location->from_edge(this->entity->location->pos.draw);

	// return the targeted unit
	return target_ptr;
}

UnitReference TargetAction::get_target() const {
	return this->target;
}

void TargetAction::set_target(UnitReference new_target) {
	this->target = new_target;
	this->update_distance();
}


DecayAction::DecayAction(Unit *e)
	:
	UnitAction(e, graphic_type::standing),
	end_frame{.0f} {

	auto &g_set = this->current_graphics();
	if (g_set.count(this->graphic) > 0) {
		this->end_frame = g_set.at(this->graphic)->frame_count - 1;
	}
}

void DecayAction::update(unsigned int time) {
	this->frame += time * this->frame_rate / 10000.0f;
}

void DecayAction::on_completion() {}

bool DecayAction::completed() const {
	return this->frame > this->end_frame;
}

DeadAction::DeadAction(Unit *e, std::function<void()> on_complete)
	:
	UnitAction(e, graphic_type::dying),
	end_frame{.0f},
	on_complete_func{on_complete} {

	auto &g_set = this->current_graphics();
	if (g_set.count(graphic) > 0) {
		this->end_frame = g_set.at(graphic)->frame_count - 1;
	}
}

void DeadAction::update(unsigned int time) {
	if (this->entity->has_attribute(attr_type::damaged)) {
		auto &dm = this->entity->get_attribute<attr_type::damaged>();
		dm.hp = 0;
	}

	// inc frame but do not pass the end frame
	// the end frame will remain if the object carries resources
	if (this->frame < this->end_frame) {
		this->frame += 0.001 + time * this->frame_rate / 3.0f;
	}
	else {
		this->frame = this->end_frame;
	}
}

void DeadAction::on_completion() {
	this->on_complete_func();
}

bool DeadAction::completed() const {

	// check resource, trees/huntables with resource are not removed but not workers
	if (this->entity->has_attribute(attr_type::resource) && !this->entity->has_attribute(attr_type::worker)) {
		auto &res_attr = this->entity->get_attribute<attr_type::resource>();
		return res_attr.amount <= 0; // cannot complete when resource remains
	}
	return this->frame > this->end_frame;
}

FoundationAction::FoundationAction(Unit *e, bool add_destruction)
	:
	UnitAction(e, graphic_type::construct),
	add_destruct_effect{add_destruction},
	cancel{false} {
}

void FoundationAction::update(unsigned int) {
	if (!this->entity->location) {
		this->cancel = true;
	}
}

void FoundationAction::on_completion() {

	// do nothing if construction is cancelled
	if (this->cancel) {
		return;
	}

	// add destruction effect when available
	if (this->add_destruct_effect) {
		this->entity->push_action(std::make_unique<DeadAction>(this->entity), true);
	}
	this->entity->push_action(std::make_unique<IdleAction>(this->entity), true);
}

bool FoundationAction::completed() const {
	return this->cancel ||
	       (this->entity->has_attribute(attr_type::building) &&
	       (this->entity->get_attribute<attr_type::building>().completed >= 1.0f));
}

IdleAction::IdleAction(Unit *e)
	:
	UnitAction(e, graphic_type::standing) {
	auto terrain = this->entity->location->get_terrain();
	auto current_tile = this->entity->location->pos.draw.to_tile3().to_tile();
	this->search = std::make_shared<TerrainSearch>(terrain, current_tile, 5.0f);

	// currently allow attack and heal automatically
	this->auto_abilities = UnitAbility::set_from_list({ability_type::attack, ability_type::heal});
}

void IdleAction::update(unsigned int time) {

	// auto task searching
	if (this->entity->location &&
	    this->entity->has_attribute(attr_type::owner) &&
	    this->entity->has_attribute(attr_type::attack) &&
	    this->entity->get_attribute<attr_type::attack>().stance != attack_stance::do_nothing) {

		// restart search from new tile when moved
		auto terrain = this->entity->location->get_terrain();
		auto current_tile = this->entity->location->pos.draw.to_tile3().to_tile();
		if (!(current_tile == this->search->start_tile())) {
			this->search = std::make_shared<TerrainSearch>(terrain, current_tile, 5.0f);
		}

		// search one tile per update
		// next tile will always be valid
		coord::tile tile = this->search->next_tile();
		auto tile_data = terrain->get_data(tile);
		auto &player = this->entity->get_attribute<attr_type::owner>().player;

		// find and actions which can be invoked
		for (auto object_location : tile_data->obj) {
			Command to_object(player, &object_location->unit);

			// only allow abilities in the set of auto ability types
			to_object.set_ability_set(auto_abilities);
			if (this->entity->queue_cmd(to_object)) {
				break;
			}
		}
	}

	// unit carrying ressources take the carrying sprite when idle
	// we're not updating frames because the carying sprite is walking
	if (entity->has_attribute(attr_type::worker)) {
		auto &worker_resource = this->entity->get_attribute<attr_type::resource>();
		if (worker_resource.amount > 0) {
			this->graphic = graphic_type::carrying;
		} else {
			this->graphic = graphic_type::standing;
			this->frame += time * this->frame_rate / 20.0f;
		}
	}
	else {
		// inc frame
		this->frame += time * this->frame_rate / 20.0f;
	}
}

void IdleAction::on_completion() {}

bool IdleAction::completed() const {
	if (this->entity->has_attribute(attr_type::damaged)) {
		auto &dm = this->entity->get_attribute<attr_type::damaged>();
		return dm.hp == 0;
	}
	else if (this->entity->has_attribute(attr_type::resource)) {
		auto &res_attr = this->entity->get_attribute<attr_type::resource>();
		return res_attr.amount <= 0.0;
	}
	return false;
}

MoveAction::MoveAction(Unit *e, coord::phys3 tar, bool repath)
	:
	UnitAction{e, graphic_type::walking},
	unit_target{},
	target(tar),
	radius{path::path_grid_size},
	allow_repath{repath},
	end_action{false} {
	this->initialise();
}

MoveAction::MoveAction(Unit *e, UnitReference tar, coord::phys_t within_range)
	:
	UnitAction{e, graphic_type::walking},
	unit_target{tar},
	target(tar.get()->location->pos.draw),
	radius{within_range},
	allow_repath{false},
	end_action{false} {
	this->initialise();
}

void MoveAction::initialise() {
	// switch workers to the carrying graphic
	if (this->entity->has_attribute(attr_type::worker)) {
		auto &worker_resource = this->entity->get_attribute<attr_type::resource>();
		if (worker_resource.amount > 0) {
			this->graphic = graphic_type::carrying;
		}
	}

	// set initial distance
	this->set_distance();

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
		auto &target_object = this->unit_target.get()->location;

		// check for garrisoning objects
		if (!target_object) {
			this->end_action = true;
			return;
		}
		coord::phys3 &target_pos = target_object->pos.draw;
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

	// path not found
	if (this->path.waypoints.empty()) {
		if (!this->allow_repath) {
			this->entity->log(MSG(dbg) << "Path not found -- drop action");
			this->end_action = true;
		}
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
		this->set_distance();
	}
	else {
		// cases for modifying path when blocked
		if (this->allow_repath) {
			this->entity->log(MSG(dbg) << "Path blocked -- finding new path");
			this->set_path();
		}
		else {
			this->entity->log(MSG(dbg) << "Path blocked -- drop action");
			this->end_action = true;
		}
	}

	// inc frame
	this->frame += time * this->frame_rate / 5.0f;
}

void MoveAction::on_completion() {}

bool MoveAction::completed() const {

	// no more waypoints to a static location
	if (this->end_action ||
	    (!this->unit_target.is_valid() &&
	    this->path.waypoints.empty())) {
		return true;
	}

	//close enough to end action
	if (this->distance_to_target < this->radius) {
		return true;
	}
	return false;
}


coord::phys3 MoveAction::next_waypoint() const {
	if (this->path.waypoints.size() > 0) {
		return this->path.waypoints.back().position;
	} else {
		throw Error{MSG(err) << "No next waypoint available!"};
	}
}


void MoveAction::set_path() {
	if (this->unit_target.is_valid()) {
		this->path = path::to_object(this->entity->location.get(), this->unit_target.get()->location.get(), this->radius);
	}
	else {
		coord::phys3 start = this->entity->location->pos.draw;
		coord::phys3 end = this->target;
		this->path = path::to_point(start, end, this->entity->location->passable);
	}
}

void MoveAction::set_distance() {
	if (this->unit_target.is_valid()) {
		auto &target_object = this->unit_target.get()->location;
		coord::phys3 &unit_pos = this->entity->location->pos.draw;
		this->distance_to_target = target_object->from_edge(unit_pos);
	}
	else {
		coord::phys3_delta move_dir = this->target - this->entity->location->pos.draw;
		this->distance_to_target = static_cast<coord::phys_t>(std::hypot(move_dir.ne, move_dir.se));
	}
}

GarrisonAction::GarrisonAction(Unit *e, UnitReference build)
	:
	TargetAction{e, graphic_type::standing, build},
	complete{false} {
}

void GarrisonAction::update_in_range(unsigned int, Unit *target_unit) {
	auto &garrison_attr = target_unit->get_attribute<attr_type::garrison>();
	garrison_attr.content.push_back(this->entity->get_ref());

	if (this->entity->location) {
		this->entity->location->remove();
		this->entity->location = nullptr;
	}
	this->complete = true;
}

UngarrisonAction::UngarrisonAction(Unit *e, const coord::phys3 &pos)
	:
	UnitAction{e, graphic_type::standing},
	position(pos),
	complete{false} {
}

void UngarrisonAction::update(unsigned int) {
	auto &garrison_attr = this->entity->get_attribute<attr_type::garrison>();

	// try unload all objects currently garrisoned
	auto position_it = std::remove_if(
		std::begin(garrison_attr.content),
		std::end(garrison_attr.content),
		[this](UnitReference &u) {
			if (u.is_valid()) {

				// ptr to unit being ungarrisoned
				Unit *unit_ptr = u.get();

				// make sure it was placed outside
				if (unit_ptr->unit_type->place_beside(unit_ptr, this->entity->location.get())) {

					// task unit to move to position
					auto &player = this->entity->get_attribute<attr_type::owner>().player;
					Command cmd(player, this->position);
					cmd.set_ability(ability_type::move);
					unit_ptr->queue_cmd(cmd);
					return true;
				}
			}
			return false;
		});

	// remove elements which were ungarrisoned
	garrison_attr.content.erase(position_it, std::end(garrison_attr.content));

	// completed when no units are remaining
	this->complete = garrison_attr.content.empty();
}

void UngarrisonAction::on_completion() {}

TrainAction::TrainAction(Unit *e, UnitType *pp)
	:
	UnitAction{e, graphic_type::standing},
	trained{pp},
	complete{false},
	train_percent{.0f} {
}

void TrainAction::update(unsigned int time) {

	// place unit when ready
	if (this->train_percent > 1.0f) {

		// create using the producer
		UnitContainer *container = this->entity->get_container();
		auto &player = this->entity->get_attribute<attr_type::owner>().player;
		auto uref = container->new_unit(*this->trained, player, this->entity->location.get());

		// make sure unit got placed
		// try again next update if cannot place
		if (uref.is_valid()) {
			if (this->entity->has_attribute(attr_type::building)) {

				// use a move command to the gather point
				auto &build_attr = this->entity->get_attribute<attr_type::building>();
				Command cmd(player, build_attr.gather_point);
				cmd.set_ability(ability_type::move);
				uref.get()->queue_cmd(cmd);
			}
			this->complete = true;
		}
	}
	else {
		this->train_percent += 0.001 * time;
	}
}

void TrainAction::on_completion() {}

BuildAction::BuildAction(Unit *e, UnitReference foundation)
	:
	TargetAction{e, graphic_type::work, foundation},
	complete{.0f},
	build_rate{.0001f} {

	// update the units type
	auto &worker = this->entity->get_attribute<attr_type::worker>();
	auto building_class = gamedata::unit_classes::BUILDING;
	if (worker.graphics.count(building_class) > 0) {
		auto *new_type = worker.graphics.at(building_class);
		auto &pl_attr = this->entity->get_attribute<attr_type::owner>();
		new_type->initialise(this->entity, pl_attr.player);
	}
}

void BuildAction::update_in_range(unsigned int time, Unit *target_unit) {
	if (target_unit->has_attribute(attr_type::building)) {
		auto &build = target_unit->get_attribute<attr_type::building>();

		// upgrade floating outlines
		auto target_location = target_unit->location.get();
		if (target_location->is_floating()) {

			// try to place the object
			if (target_location->place(object_state::placed)) {

				// modify ground terrain
				if (build.foundation_terrain > 0) {
					target_location->set_ground(build.foundation_terrain, 0);
				}
			}
			else {

				// failed to start construction
				this->complete = 1.0f;
				return;
			}
		}

		// increment building completion
		build.completed += build_rate * time;
		this->complete = build.completed;

		if (this->complete >= 1.0f) {
			this->complete = build.completed = 1.0f;
			target_location->place(build.completion_state);
		}
	}
	else {
		this->complete = 1.0f;
	}

	// inc frame
	this->frame += time * this->frame_rate / 2.5f;
}

void BuildAction::on_completion() {
	if (this->get_target().get()->get_attribute<attr_type::building>().completed < 1.0f) {
		// The BuildAction was just aborted and we shouldn't look for new buildings
		return;
	}
	this->entity->log(MSG(dbg) << "Done building, searching for new building");
	auto valid = [this](const TerrainObject &obj) {
		if (!obj.unit.has_attribute(attr_type::building) ||
		    obj.unit.get_attribute<attr_type::building>().completed >= 1.0f) {
			return false;
		}
		this->entity->log(MSG(dbg) << "Found unit " << obj.unit.logsource_name());
		return true;
	};

	TerrainObject *new_target = find_in_radius(*this->entity->location, valid, BuildAction::search_tile_distance);
	if (new_target != nullptr) {
		this->entity->log(MSG(dbg) << "Found new building, queueing command");
		Command cmd(this->entity->get_attribute<attr_type::owner>().player, &new_target->unit);
		this->entity->queue_cmd(cmd);
	} else {
		this->entity->log(MSG(dbg) << "Didn't find new building");
	}
}

const graphic_set &BuildAction::current_graphics() const {
	if (this->entity->has_attribute(attr_type::worker)) {

		// the worker attributes attached to the unit
		// are used to modify the graphic
		auto &worker = this->entity->get_attribute<attr_type::worker>();

		if (this->get_target().is_valid() &&
		    this->get_target().get()->has_attribute(attr_type::building)) {

			// set builder graphics if available
			if (worker.graphics.count(gamedata::unit_classes::BUILDING) > 0) {
				return worker.graphics[gamedata::unit_classes::BUILDING]->graphics;
			}
		}
	}
	return this->entity->unit_type->graphics;
}

RepairAction::RepairAction(Unit *e, UnitReference tar)
	:
	TargetAction{e, graphic_type::work, tar},
	complete{false},
	time{80},
	time_left{0} {

	if (!tar.is_valid()) {
		// the target no longer exists
		complete = true;
	}
	else {

		Unit *target = tar.get();

		if (!target->has_attribute(attr_type::building)) {
			this->time *= 4;
		}

		// cost formula: 0.5 * (target cost) / (target max hp)
		auto &hp = target->get_attribute<attr_type::hitpoints>();

		// TODO get the target unit's cost
		//this->cost += get target cost;
		this->cost[game_resource::wood] = 100; // temp

		this->cost *= 0.5 / hp.hp;
	}
}

void RepairAction::update_in_range(unsigned int time, Unit *target_unit) {

	auto &hp = target_unit->get_attribute<attr_type::hitpoints>();
	auto &dm = target_unit->get_attribute<attr_type::damaged>();
	auto &owner = this->entity->get_attribute<attr_type::owner>();

	if (dm.hp >= hp.hp) {
		// repaired by something else
		this->complete = true;
	}
	else if (this->time_left > 0) {
		this->time_left -= time;

		if (this->time_left <= 0) {
			dm.hp += 1;

			if (dm.hp >= hp.hp) {
				this->complete = true;
			}
		}
	}

	if (this->time_left <= 0 && !this->complete) {
		if (owner.player.deduct(this->cost)) {
			this->time_left += this->time;
		}
		else {
			// no resources to continue
			this->complete = true;
		}
	}

	// inc frame
	this->frame += time * this->frame_rate / 2.5f;
}

GatherAction::GatherAction(Unit *e, UnitReference tar)
	:
	TargetAction{e, graphic_type::work, tar},
	complete{false},
	target_resource{true},
	target{tar} {

	Unit *target = this->target.get();
	this->resource_class = target->unit_type->unit_class;
	auto &worker = this->entity->get_attribute<attr_type::worker>();
	auto &worker_resource = this->entity->get_attribute<attr_type::resource>();

	// handle unit type changes based on resource class
	if (worker.graphics.count(this->resource_class) > 0) {
		auto *new_type = worker.graphics.at(this->resource_class);
		auto &pl_attr = this->entity->get_attribute<attr_type::owner>();
		new_type->initialise(this->entity, pl_attr.player);
	}

	// set the type of gatherer
	if (target->has_attribute(attr_type::resource)) {
		auto &resource_attr = target->get_attribute<attr_type::resource>();
		if (worker_resource.resource_type != resource_attr.resource_type) {
			worker_resource.amount = 0;
		}
		worker_resource.resource_type = resource_attr.resource_type;
	} else {
		throw std::invalid_argument("Unit reference has no resource attribute");
	}
}

GatherAction::~GatherAction() {}

void GatherAction::update_in_range(unsigned int time, Unit *targeted_resource) {
	auto &worker = this->entity->get_attribute<attr_type::worker>();
	auto &worker_resource = this->entity->get_attribute<attr_type::resource>();
	if (this->target_resource) {

		// the targets attributes
		if (!targeted_resource->has_attribute(attr_type::resource)) {
			complete = true;
			return;
		}

		// attack objects which have hitpoints (trees, hunt, sheep)
		if (this->entity->has_attribute(attr_type::owner) &&
		    targeted_resource->has_attribute(attr_type::damaged)) {
			auto &pl = this->entity->get_attribute<attr_type::owner>();
			auto &dm = targeted_resource->get_attribute<attr_type::damaged>();

			// only attack if hitpoints remain
			if (dm.hp > 0) {
				Command cmd(pl.player, targeted_resource);
				cmd.set_ability(ability_type::attack);
				cmd.add_flag(command_flag::attack_res);
				this->entity->queue_cmd(cmd);
				return;
			}
		}

		// need to return to dropsite
		if (worker_resource.amount > worker.capacity) {

			// move to dropsite location
			this->target_resource = false;
			this->set_target(this->nearest_dropsite(worker_resource.resource_type));
		}
		else {

			auto &resource_attr = targeted_resource->get_attribute<attr_type::resource>();
			if (resource_attr.amount <= 0.0) {

				// when the resource runs out
				if (worker_resource.amount > 0.0) {
					this->target_resource = false;
					this->set_target(this->nearest_dropsite(worker_resource.resource_type));
				}
				else {
					this->complete = true;
				}
			}
			else {

				// transfer using gather rate
				double amount = worker.gather_rate * time;
				worker_resource.amount += amount;
				resource_attr.amount -= amount;
			}
		}
	}
	else {

		// dropsite has been reached
		// add value to player stockpile
		Player &player = this->entity->get_attribute<attr_type::owner>().player;
		player.receive(worker_resource.resource_type, worker_resource.amount);
		worker_resource.amount = 0.0;

		// make sure the resource stil exists
		if (this->target.is_valid() &&
		    this->target.get()->get_attribute<attr_type::resource>().amount > 0.0) {

			// return to resouce collection
			this->target_resource = true;
			this->set_target(this->target);
		}
		else {

			// resource depleted
			this->complete = true;
		}
	}

	// inc frame
	this->frame += time * this->frame_rate / 3.0f;
}

UnitReference GatherAction::nearest_dropsite(game_resource res_type) {

	// find nearest dropsite from the targeted resource
	auto ds = find_near(*this->target.get()->location,
		[=](const TerrainObject &obj) {

			if (not obj.unit.has_attribute(attr_type::building) or &obj.unit == this->entity or &obj.unit == this->target.get()) {
				return false;
			}

			return obj.unit.get_attribute<attr_type::building>().completed >= 1.0f &&
			       obj.unit.has_attribute(attr_type::owner) &&
			       obj.unit.get_attribute<attr_type::owner>().player.owns(*this->entity) &&
			       obj.unit.has_attribute(attr_type::dropsite) &&
			       obj.unit.get_attribute<attr_type::dropsite>().accepting_resource(res_type);
	});

	if (ds) {
		return ds->unit.get_ref();
	}
	else {
		this->entity->log(MSG(dbg) << "no dropsite found");
		return UnitReference();
	}
}

const graphic_set &GatherAction::current_graphics() const {
	return this->entity->unit_type->graphics;
}

AttackAction::AttackAction(Unit *e, UnitReference tar)
	:
	TargetAction{e, graphic_type::attack, tar, get_attack_range(e)},
	strike_percent{0.0f},
	rate_of_fire{0.002f} {

	// switch graphic type for villagers not collecting resources
	if (this->entity->has_attribute(attr_type::worker) &&
	    (!tar.get()->has_attribute(attr_type::resource) || tar.get()->has_attribute(attr_type::worker))) {
		auto &att_attr = this->entity->get_attribute<attr_type::attack>();
		auto &pl_attr = this->entity->get_attribute<attr_type::owner>();
		att_attr.attack_type->initialise(this->entity, pl_attr.player);
	}
}

AttackAction::~AttackAction() {}

void AttackAction::update_in_range(unsigned int time, Unit *target_ptr) {
	if (this->strike_percent > 0.0) {
		this->strike_percent -= this->rate_of_fire * time;
	}
	else {
		this->strike_percent += 1.0f;
		this->attack(*target_ptr);
	}

	// inc frame
	this->frame += time * this->current_graphics().at(graphic)->frame_count * this->rate_of_fire;
}

bool AttackAction::completed_in_range(Unit *target_ptr) const {
	auto &dm = target_ptr->get_attribute<attr_type::damaged>();
	return dm.hp < 1; // is unit still alive?
}

void AttackAction::attack(Unit &target) {
	auto &attack = this->entity->get_attribute<attr_type::attack>();
	if (attack.ptype) {

		// add projectile to the game
		this->fire_projectile(attack, target.location->pos.draw);
	}
	else {
		this->damage_object(target);
	}
}

void AttackAction::fire_projectile(const Attribute<attr_type::attack> &att, const coord::phys3 &target) {

	// container terrain and initial position
	UnitContainer *container = this->entity->get_container();
	coord::phys3 current_pos = this->entity->location->pos.draw;
	current_pos.up = att.init_height;

	// create using the producer
	auto player = this->entity->get_attribute<attr_type::owner>().player;
	auto projectile_ref = container->new_unit(*att.ptype, player, current_pos);

	// send towards target using a projectile ability (creates projectile motion action)
	if (projectile_ref.is_valid()) {
		auto projectile = projectile_ref.get();
		auto &projectile_attr = projectile->get_attribute<attr_type::projectile>();
		projectile_attr.launcher = this->entity->get_ref();
		projectile_attr.launched = true;
		projectile->push_action(std::make_unique<ProjectileAction>(projectile, target), true);
	}
	else {
		this->entity->log(MSG(dbg) << "projectile launch failed");
	}
}


HealAction::HealAction(Unit *e, UnitReference tar)
	:
	TargetAction{e, graphic_type::heal, tar, get_attack_range(e)},
	heal_percent{0.0f} {

}

HealAction::~HealAction() {}

void HealAction::update_in_range(unsigned int time, Unit *target_ptr) {
	auto &heal = this->entity->get_attribute<attr_type::heal>();

	if (this->heal_percent > 0.0) {
		this->heal_percent -= heal.rate * time;
	}
	else {
		this->heal_percent += 1.0f;
		this->heal(*target_ptr);
	}

	// inc frame
	this->frame += time * this->current_graphics().at(graphic)->frame_count * heal.rate;
}

bool HealAction::completed_in_range(Unit *target_ptr) const {
	auto &hp = target_ptr->get_attribute<attr_type::hitpoints>();
	auto &dm = target_ptr->get_attribute<attr_type::damaged>();
	return dm.hp >= hp.hp; // is unit at full hitpoints?
}

void HealAction::heal(Unit &target) {
	auto &heal = this->entity->get_attribute<attr_type::heal>();

	// TODO move to seperate function heal_object (like damage_object)?
	// heal object
	if (target.has_attribute(attr_type::hitpoints) && target.has_attribute(attr_type::damaged)) {
		auto &hp = target.get_attribute<attr_type::hitpoints>();
		auto &dm = target.get_attribute<attr_type::damaged>();
		if ((dm.hp + heal.life) < hp.hp) {
			dm.hp += heal.life;
		}
		else {
			dm.hp = hp.hp;
		}
	}

}


ConvertAction::ConvertAction(Unit *e, UnitReference tar)
	:
	TargetAction{e, graphic_type::attack, tar},
	complete{.0f} {
}

void ConvertAction::update_in_range(unsigned int, Unit *) {}

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

		// find object which was hit
		auto terrain = this->entity->location->get_terrain();
		TileContent *tc = terrain->get_data(new_position.to_tile3().to_tile());
		if (tc && !tc->obj.empty()) {
			for (auto obj_location : tc->obj) {
				if (this->entity->location.get() != obj_location &&
				    obj_location->check_collisions()) {
					this->damage_object(obj_location->unit, 1);
					break;
				}
			}
		}

		has_hit = true;
	}

	// inc frame
	this->frame += time * this->frame_rate;
}

void ProjectileAction::on_completion() {}

bool ProjectileAction::completed() const {
	return (has_hit || this->entity->location->pos.draw.up <= 0);
}

} /* namespace openage */
