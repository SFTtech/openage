// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include <algorithm>
#include <cmath>

#include "../player.h"
#include "../pathfinding/a_star.h"
#include "../pathfinding/heuristics.h"
#include "../terrain/terrain.h"
#include "action.h"
#include "command.h"
#include "producer.h"
#include "unit.h"
#include "unit_texture.h"

namespace openage {

bool UnitAction::show_debug = false;

UnitAction::UnitAction(Unit *u, graphic_type initial_gt)
	:
	UnitAction(u, initial_gt, 3 * path::path_grid_size + (u->location->min_axis() / 2)) {
}

UnitAction::UnitAction(Unit *u, graphic_type initial_gt, coord::phys_t action_radius)
	:
	entity{u},
	radius{action_radius},
	graphic{initial_gt},
	frame{.0f},
	frame_rate{.0f} {

	if (u->graphics->count(initial_gt) > 0) {
		this->frame_rate = u->graphics->at(initial_gt)->frame_rate;
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
	if(show_debug && this->debug_draw_action) {
		this->debug_draw_action();
	}
}

void UnitAction::face_towards(const coord::phys3 pos) {
	if (this->entity->has_attribute(attr_type::direction)) {
		auto &d_attr = this->entity->get_attribute<attr_type::direction>();
		d_attr.unit_dir = pos - this->entity->location->pos.draw;
	}
}

void UnitAction::damage_object(Unit &target, unsigned dmg) {
	if (target.has_attribute(attr_type::hitpoints)) {
		auto &hp = target.get_attribute<attr_type::hitpoints>();
		hp.current -= dmg;
	}
}

void UnitAction::move_to(Unit &target, bool use_range) {
	auto &player = this->entity->get_attribute<attr_type::owner>().player;
	Command cmd(player, &target);
	cmd.set_ability(ability_type::move);
	if (use_range) {
		cmd.add_flag(command_flag::use_range);
	}
	this->entity->invoke(cmd);
}

DecayAction::DecayAction(Unit *e)
	:
	UnitAction(e, graphic_type::standing),
	end_frame{.0f} {

	if (this->entity->graphics->count(graphic) > 0) {
		this->end_frame = this->entity->graphics->at(graphic)->frame_count - 1;
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
	// check resource, trees/huntables with resource are not removed
	if (this->entity->has_attribute(attr_type::resource)) {
		auto &res_attr = this->entity->get_attribute<attr_type::resource>();
		return res_attr.amount <= 0; // cannot complete when resource remains
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

void IdleAction::on_completion() {}

bool IdleAction::completed() const {
	if (this->entity->has_attribute(attr_type::hitpoints)) {
		auto &hp = this->entity->get_attribute<attr_type::hitpoints>();
		return hp.current <= 0.0f;
	}
	else if (this->entity->has_attribute(attr_type::resource)) {
		auto &res_attr = this->entity->get_attribute<attr_type::resource>();
		return res_attr.amount <= 0.0f;
	}
	return false;
}

MoveAction::MoveAction(Unit *e, coord::phys3 tar, bool repath)
	:
	UnitAction{e, graphic_type::walking, path::path_grid_size},
	unit_target{},
	target(tar),
	allow_repath{repath} {
	this->initialise();
}

MoveAction::MoveAction(Unit *e, UnitReference tar, coord::phys_t within_range)
	:
	UnitAction{e, graphic_type::walking, within_range},
	unit_target{tar},
	target(tar.get()->location->pos.draw),
	allow_repath{false} {
	this->initialise();
}

void MoveAction::initialise() {
	// switch workers to the carrying graphic
	if (this->entity->has_attribute(attr_type::gatherer)) {
		auto &gather_attr = this->entity->get_attribute<attr_type::gatherer>();
		if (gather_attr.amount > 0) {
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
		auto target_object = this->unit_target.get()->location;
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
			this->path.waypoints.clear();
		}
	}

	// inc frame
	this->frame += time * this->frame_rate / 5.0f;
}

void MoveAction::on_completion() {}

bool MoveAction::completed() const {

	// no more waypoints to a static location
	if (!this->unit_target.is_valid() &&
		this->path.waypoints.empty()) {
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
		throw util::Error{MSG(err) << "No next waypoint available!"};
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
		auto target_object = this->unit_target.get()->location;
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
	UnitAction{e, graphic_type::standing},
	building{build},
	complete{false} {
}

void GarrisonAction::update(unsigned int) {
	Unit *build_ptr = building.get();

	// move to resource being grrisoned in
	this->distance_to_target = build_ptr->location->from_edge(this->entity->location->pos.draw);
	if (this->distance_to_target > this->radius) {

		// move to foundation
		this->move_to(*build_ptr, false);
		return;
	}
	else {
		auto &garrison_attr = this->building.get()->get_attribute<attr_type::garrison>();
		garrison_attr.content.push_back(this->entity->get_ref());

		if (this->entity->location) {
			this->entity->location->remove();
			this->entity->location = nullptr;
		}
		this->complete = true;
	}
}

void GarrisonAction::on_completion() {}

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
				if (unit_ptr->producer->place_beside(unit_ptr, this->entity->location)) {

					// task unit to move to position
					auto &player = this->entity->get_attribute<attr_type::owner>().player;
					Command cmd(player, this->position);
					cmd.set_ability(ability_type::move);
					unit_ptr->invoke(cmd);
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

TrainAction::TrainAction(Unit *e, UnitProducer *pp)
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
		auto uref = container->new_unit(*this->trained, player, this->entity->location);
		if (uref.is_valid()) {

			// use a move command to the gather point
			// TODO: use buildings gather point, assume {3, 3} for now
			Command cmd(player, coord::tile{3, 3}.to_phys2().to_phys3());
			cmd.set_ability(ability_type::move);
			uref.get()->invoke(cmd);

			// try again next update if cannot place
			this->complete = true;
		}
	}
	else {
		this->train_percent += 0.001 * time;
	}
}

void TrainAction::on_completion() {}

BuildAction::BuildAction(Unit *e, UnitProducer *pp, const coord::phys3 &pos)
	:
	UnitAction{e, graphic_type::work},
	building{},
	producer{pp},
	position(pos),
	complete{.0f} {
}

BuildAction::BuildAction(Unit *e, UnitReference foundation)
	:
	UnitAction{e, graphic_type::work},
	building{foundation},
	producer{nullptr},
	complete{.0f} {
}

void BuildAction::update(unsigned int time) {
	if (this->building.is_valid()) {

		// the worker attributes attached to the unit
		auto &gatherer_attr = this->entity->get_attribute<attr_type::gatherer>();

		// set relevant graphics
		this->entity->graphics = &gatherer_attr.graphics[gamedata::unit_classes::BUILDING]->graphics;

		// set direction unit should face
		Unit *b = this->building.get();
		auto target_location = b->location;
		this->face_towards(target_location->pos.draw);

		// move to resource being collected
		auto distance_to_foundation = target_location->from_edge(this->entity->location->pos.draw);
		if (distance_to_foundation > this->radius) {

			// move to foundation
			this->move_to(*b);
			return;
		}
		else if (b->has_attribute(attr_type::building)) {
			// increment building completion
			auto &build = b->get_attribute<attr_type::building>();
			build.completed += 0.001;
			this->complete = build.completed;
		}
		else {
			this->complete = 1.0;
		}
	}
	else {

		// create foundation using the producer
		UnitContainer *container = this->entity->get_container();
		auto &player = this->entity->get_attribute<attr_type::owner>().player;
		this->building = container->new_unit(*this->producer, player, this->position);
	}

	// inc frame
	this->frame += time * this->frame_rate / 3.0f;
}

void BuildAction::on_completion() {
	// TODO: help build nearby buildings
}

RepairAction::RepairAction(Unit *e, UnitReference tar)
	:
	UnitAction{e, graphic_type::work},
	target{tar},
	complete{false} {
}

void RepairAction::update(unsigned int) {
	// todo
}

void RepairAction::on_completion() {}

GatherAction::GatherAction(Unit *e, UnitReference tar)
	:
	UnitAction{e, graphic_type::work},
	complete{false},
	target{tar},
	dropsite{} {

	// find nearest dropsite from the targeted resource
	TerrainObject *ds = path::find_nearest(this->target.get()->location.get(),
		[=](const openage::TerrainObject *other) -> bool {
			if (!other) {
				return false;
			}
			return &other->unit != this->entity &&
			       &other->unit != tar.get() &&
			       other->unit.has_attribute(attr_type::building);
	});

	if (ds) {
		this->entity->log(MSG(dbg) << "dropsite");
		this->dropsite = ds->unit.get_ref();
	}
	else {
		this->entity->log(MSG(dbg) << "no dropsite found");
	}
}

GatherAction::~GatherAction() {}

void GatherAction::update(unsigned int time) {
	if (!this->target.is_valid() || 
	    !this->dropsite.is_valid()) {
		complete = true;
	    return;
	}
	auto targeted_resource = this->target.get();

	// the targets attributes
	if (!targeted_resource->has_attribute(attr_type::resource)) {
		complete = true;
		return;
	}
	auto &resource_attr = targeted_resource->get_attribute<attr_type::resource>();

	// the gatherer attributes attached to the unit
	auto &gatherer_attr = this->entity->get_attribute<attr_type::gatherer>();
	gatherer_attr.current_type = resource_attr.resource_type;

	// set relevant graphics if available
	auto class_type = targeted_resource->unit_class;
	if (gatherer_attr.graphics.count(class_type) > 0) {
		this->entity->graphics = &gatherer_attr.graphics[class_type]->graphics;
	}

	// set direction unit should face
	auto target_location = targeted_resource->location;
	this->face_towards(target_location->pos.draw);

	// owner of gatherer
	auto &player = this->entity->get_attribute<attr_type::owner>().player;

	// return to dropsite
	if (gatherer_attr.amount > gatherer_attr.capacity) {
		// dropsite position
		auto dropsite_location = this->dropsite.get()->location;

		// move to resource being collected
		auto distance_to_dropsite = dropsite_location->from_edge(this->entity->location->pos.draw);
		if (distance_to_dropsite > this->radius) {

			// move to dropsite
			this->move_to(*this->dropsite.get());
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
			Command cmd(player, this->target.get());
			cmd.set_ability(ability_type::attack);
			this->entity->invoke(cmd);
			return;
		}
	}

	// move to resource being collected
	this->distance_to_target = target_location->from_edge(this->entity->location->pos.draw);
	if (distance_to_target > this->radius) {
		this->move_to(*this->target.get());
		return;
	}

	// transfer using gather rate
	gatherer_attr.amount += gatherer_attr.gather_rate * time;
	resource_attr.amount -= gatherer_attr.gather_rate * time;

	// inc frame
	this->frame += time * this->frame_rate / 3.0f;
}

void GatherAction::on_completion() {
	// TODO: find another resource to gather
}

bool GatherAction::completed() const {
	return this->complete;
}

AttackAction::AttackAction(Unit *e, UnitReference tar)
	:
	UnitAction{e, graphic_type::attack},
	target{tar},
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
	else if (distance_to_target <= this->radius) {
		strike_percent = 1.0f;
		this->frame = 0.0f;
		this->attack(*this->target.get());
	}
	else {
		// out of range so try move towards
		// if this unit has a move ability
		this->move_to(*this->target.get());
	}
}

void AttackAction::on_completion() {}

bool AttackAction::completed() const {
	if (!this->target.is_valid()) return true;
	auto &h_attr = this->target.get()->get_attribute<attr_type::hitpoints>();
	return h_attr.current < 1; // is unit still alive?
}

void AttackAction::attack(Unit &target) {
	auto &attack = this->entity->get_attribute<attr_type::attack>();
	if (attack.pp) {

		// add projectile to the game
		this->fire_projectile(attack, target.location->pos.draw);
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
	coord::phys3 current_pos = this->entity->location->pos.draw;
	current_pos.up = att.init_height;

	// create using the producer
	auto player = this->entity->get_attribute<attr_type::owner>().player;
	auto projectile_ref = container->new_unit(*att.pp, player, current_pos);

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

ConvertAction::ConvertAction(Unit *e, UnitReference tar)
	:
	UnitAction{e, graphic_type::attack},
	target{tar},
	complete{.0f} {
}

void ConvertAction::update(unsigned int) {
	// todo
}

void ConvertAction::on_completion() {}

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
		Terrain *terrain = this->entity->location->get_terrain();
		TileContent *tc = terrain->get_data(new_position.to_tile3().to_tile());
		if (tc && !tc->obj.empty()) {
			for (auto item : tc->obj) {
				auto obj_location = item.lock();
				if (this->entity->location != obj_location) {
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
