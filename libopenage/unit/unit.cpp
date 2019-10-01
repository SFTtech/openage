// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#include <algorithm>
#include <cmath>
#include <limits>

#include "../terrain/terrain.h"
#include "../engine.h"

#include "ability.h"
#include "action.h"
#include "command.h"
#include "producer.h"
#include "unit.h"
#include "unit_texture.h"

namespace openage {

Unit::Unit(UnitContainer *c, id_t id)
	:
	id{id},
	unit_type{nullptr},
	selected{false},
	pop_destructables{false},
	container(c) {}

Unit::~Unit() {

	// remove any used location from the map
	if (this->location) {
		this->location->remove();
	}
}

void Unit::reset() {
	this->ability_available.clear();
	this->action_stack.clear();
	this->pop_destructables = false;
}

bool Unit::has_action() const {
	return !this->action_stack.empty();
}

bool Unit::accept_commands() const {
	return (this->has_action() && this->top()->allow_control());
}

bool Unit::is_own_unit(const Player &player) {
	return player.owns(*this);
}

UnitAction *Unit::top() const {
	if (this->action_stack.empty()) {
		throw Error{MSG(err) << "Unit stack empty - no top action exists"};
	}
	return this->action_stack.back().get();
}

UnitAction *Unit::before(const UnitAction *action) const {
	auto start = std::find_if(
		std::begin(this->action_stack),
		std::end(this->action_stack),
		[action](const std::unique_ptr<UnitAction> &a) {
			return action == a.get();
		});

	if (start != std::begin(this->action_stack) &&
	    start != std::end(this->action_stack)) {
		return (*(start - 1)).get();
	}
	return nullptr;
}

bool Unit::update(time_nsec_t lastframe_duration) {

	// if unit is not on the map then do nothing
	if (!this->location) {
		return true;
	}

	// unit is dead (not player controlled)
	if (this->pop_destructables) {
		this->erase_after(
			[](std::unique_ptr<UnitAction> &e) {
				return e->allow_interrupt() || e->allow_control();
			}, false);
	}

	/*
	 * the active action is on top
	 */
	if (this->has_action()) {
		// TODO: change the entire unit action timing
		//       to a higher resolution like nsecs or usecs.

		// time as float, in milliseconds.
		auto time_elapsed = lastframe_duration / 1e6;

		this->top()->update(time_elapsed);

		// the top primary action specifies whether
		// secondary actions are updated
		if (this->top()->allow_control()) {
			this->update_secondary(time_elapsed);
		}

		// check completion of all primary actions,
		// pop completed actions and anything above
		this->erase_after(
			[](std::unique_ptr<UnitAction> &e) {
				return e->completed();
			});
	}

	// apply new queued commands
	this->apply_all_cmds();

	return true;
}

void Unit::update_secondary(int64_t time_elapsed) {
	// update secondary actions and remove when completed
	auto position_it = std::remove_if(
		std::begin(this->action_secondary),
		std::end(this->action_secondary),
		[time_elapsed](std::unique_ptr<UnitAction> &action) {
			action->update(time_elapsed);
			return action->completed();
		});
	this->action_secondary.erase(position_it, std::end(this->action_secondary));
}

void Unit::apply_all_cmds() {
	std::lock_guard<std::mutex> lock(this->command_queue_lock);
	while (!this->command_queue.empty()) {
		auto &action = this->command_queue.front();
		this->apply_cmd(action.first, action.second);
		this->command_queue.pop();
	}
}


void Unit::apply_cmd(std::shared_ptr<UnitAbility> ability, const Command &cmd) {
	// if the interrupt flag is set, discard ongoing actions
	bool is_direct = cmd.has_flag(command_flag::interrupt);
	if (is_direct) {
		this->stop_actions();
	}
	if (ability->can_invoke(*this, cmd)) {
		ability->invoke(*this, cmd, is_direct);
	}
}


void Unit::draw(const Engine &engine) {

	// the top action decides the graphic type and action
	this->draw(this->location.get(), this->top()->current_graphics(), engine);
}


void Unit::draw(TerrainObject *loc, const graphic_set &grpc, const Engine &engine) {
	ENSURE(loc != nullptr, "there should always be a location for a placed unit");

	auto top_action = this->top();
	auto draw_graphic = top_action->type();
	if (grpc.count(draw_graphic) == 0) {
		this->log(MSG(warn) << "Graphic not available");
		return;
	}

	// the texture to draw with
	auto draw_texture = grpc.at(draw_graphic);
	if (!draw_texture) {
		this->log(MSG(warn) << "Graphic null");
		return;
	}

	// frame specified by the current action
	auto draw_frame = top_action->current_frame();
	this->draw(loc->pos.draw, draw_texture, draw_frame, engine);

	// draw a shadow if the graphic is available
	if (grpc.count(graphic_type::shadow) > 0) {
		auto draw_shadow = grpc.at(graphic_type::shadow);
		if (draw_shadow) {

			// position without height component
			// TODO: terrain elevation
			coord::phys3 shadow_pos = loc->pos.draw;
			shadow_pos.up = 0;
			this->draw(shadow_pos, draw_shadow, draw_frame, engine);
		}
	}

	// draw debug details
	top_action->draw_debug(engine);
}


void Unit::draw(coord::phys3 draw_pos, std::shared_ptr<UnitTexture> graphic, unsigned int frame, const Engine &engine) {

	// players color if available
	unsigned color = 0;
	if (this->has_attribute(attr_type::owner)) {
		auto &own_attr = this->get_attribute<attr_type::owner>();
		color = own_attr.player.color;
	}

	// check if object has a direction
	if (this->has_attribute(attr_type::direction)) {

		// directional textures
		auto &d_attr = this->get_attribute<attr_type::direction>();
		coord::phys3_delta draw_dir = d_attr.unit_dir;
		graphic->draw(engine.coord, draw_pos.to_camgame(engine.coord), draw_dir, frame, color);
	}
	else {
		graphic->draw(engine.coord, draw_pos.to_camgame(engine.coord), frame, color);
	}
}

void Unit::give_ability(std::shared_ptr<UnitAbility> ability) {
	this->ability_available.emplace(std::make_pair(ability->type(), ability));
}

UnitAbility *Unit::get_ability(ability_type type) {
	if (this->ability_available.count(type) > 0) {
		return this->ability_available[type].get();
	}
	return nullptr;
}

void Unit::push_action(std::unique_ptr<UnitAction> action, bool force) {
	// unit not being deleted -- can control unit
	if (force || this->accept_commands()) {
	    this->action_stack.push_back(std::move(action));
	}
}

void Unit::secondary_action(std::unique_ptr<UnitAction> action) {
	this->action_secondary.push_back(std::move(action));
}

void Unit::add_attribute(std::shared_ptr<AttributeContainer> attr) {
	this->attributes.add(attr);
}

void Unit::add_attributes(const Attributes &attr) {
	this->attributes.add_copies(attr);
}

void Unit::add_attributes(const Attributes &attr, bool shared, bool unshared) {
	this->attributes.add_copies(attr, shared, unshared);
}

bool Unit::has_attribute(attr_type type) const {
	return this->attributes.has(type);
}

std::shared_ptr<UnitAbility> Unit::queue_cmd(const Command &cmd) {
	std::lock_guard<std::mutex> lock(this->command_queue_lock);

	// following the specified ability priority
	// find suitable ability for this target if available
	for (auto &ability : ability_priority) {
		auto pair = this->ability_available.find(ability);
		if (pair != this->ability_available.end() &&
		    cmd.ability()[static_cast<int>(pair->first)] && pair->second->can_invoke(*this, cmd)) {
			command_queue.push(std::make_pair(pair->second, cmd));
			return pair->second;
		}
	}
	return nullptr;
}

void Unit::delete_unit() {
	this->pop_destructables = true;
}

void Unit::stop_gather() {
	this->erase_after(
		[](std::unique_ptr<UnitAction> &e) {
			return e->name() == "gather";
		}, false);
}

void Unit::stop_actions() {

	// work around for workers continuing to work after retasking
	if (this->has_attribute(attr_type::worker)) {
		this->stop_gather();
	}

	// discard all interruptible tasks
	this->erase_after(
		[](std::unique_ptr<UnitAction> &e) {
			return e->allow_interrupt();
		});
}

UnitReference Unit::get_ref() {
	return UnitReference(this->container, id, this);
}

UnitContainer *Unit::get_container() const {
	return this->container;
}

std::string Unit::logsource_name() {
	return "Unit " + std::to_string(this->id);
}

void Unit::erase_after(std::function<bool(std::unique_ptr<UnitAction> &)> func, bool run_completed) {
	auto position_it = std::find_if(
		std::begin(this->action_stack),
		std::end(this->action_stack),
		func);

	if (position_it != std::end(this->action_stack)) {
		auto completed_action = std::move(*position_it);

		// erase from the stack
		this->action_stack.erase(position_it, std::end(this->action_stack));

		// perform any completion actions
		if (run_completed) {
			completed_action->on_completion();
		}
	}
}

} // openage
