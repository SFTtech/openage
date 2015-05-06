// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include <algorithm>
#include <cmath>

#include "../terrain/terrain.h"
#include "../engine.h"
#include "ability.h"
#include "action.h"
#include "command.h"
#include "producer.h"
#include "unit.h"
#include "unit_texture.h"

namespace openage {

Unit::Unit(UnitContainer &c, id_t id)
	:
	id{id},
	unit_type{nullptr},
	selected{false},
	pop_destructables{false},
	container(c) {

}

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

UnitAction *Unit::top() const {
	if (this->action_stack.empty()) {
		throw util::Error{MSG(err) << "Unit stack empty - no top action exists"};
	}
	return this->action_stack.back().get();
}

bool Unit::update() {

	// if unit is not on the map then do nothing
	if (!this->location) {
		return true;
	}

	if (this->pop_destructables) {
		this->erase_after(
			[](std::unique_ptr<UnitAction> &e) {
				return e->allow_interupt() || e->allow_control();
			}, false);
	}

	/*
	 * the active action is on top
	 */
	if (this->has_action()) {
		Engine &engine = Engine::get();

		// TODO: change the entire unit action timing to a higher resolution like
		// nsecs or usecs.
		auto time_elapsed = engine.lastframe_duration_nsec() / 1e6;
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

void Unit::draw() {
	this->draw(this->location.get(), this->graphics);
}

void Unit::draw(TerrainObject *loc, graphic_set *grpc) {

	// there should always be a location
	if (!loc) {
		this->log(MSG(warn) << "Cannot draw unit with no location");
	}
	auto top_action = this->top();
	auto &draw_pos = loc->pos.draw;
	auto draw_graphic = top_action->type();
	if (!grpc || grpc->count(draw_graphic) == 0) {
		this->log(MSG(warn) << "Graphic not available");
		return;
	}

	// the texture to draw with
	auto draw_texture = grpc->at(draw_graphic);
	if (!draw_texture) {
		this->log(MSG(warn) << "Graphic null");
		return;
	}

	// frame specified by the current action
	auto draw_frame = top_action->current_frame();

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
		draw_texture->draw(draw_pos.to_camgame(), draw_dir, draw_frame, color);

		if (grpc->count(graphic_type::shadow) > 0) {
			auto unit_shadow = grpc->at(graphic_type::shadow);
			if (unit_shadow) {

				// position without height component
				coord::phys3 shadow_pos = draw_pos;
				shadow_pos.up = 0;
				unit_shadow->draw(shadow_pos.to_camgame(), draw_dir, draw_frame, color);
			}
		}
	}
	else {
		draw_texture->draw(draw_pos.to_camgame(), draw_frame, color);
	}

	top_action->draw_debug();
}

void Unit::give_ability(std::shared_ptr<UnitAbility> ability) {
	this->ability_available.insert({ability->type(), ability});
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

void Unit::add_attribute(AttributeContainer *attr) {
	this->attribute_map.insert(attr_map_t::value_type(attr->type, attr));
}

bool Unit::has_attribute(attr_type type) const {
	return (this->attribute_map.count(type) > 0);
}

bool Unit::invoke(const Command &cmd, bool direct_command) {

	// find suitable ability for this target if available
	for (auto &action : this->ability_available) {
		if (cmd.ability()[static_cast<int>(action.first)] && action.second->can_invoke(*this, cmd)) {
			if (direct_command) {

				// drop other actions if a new action is found
				this->stop_actions();
			}
			action.second->invoke(*this, cmd, direct_command);
			return true;
		}
	}
	return false;
}

void Unit::delete_unit() {
	this->pop_destructables = true;
}

void Unit::stop_actions() {
	/*
	 * discard all interruptible tasks
	 */
	this->erase_after(
		[](std::unique_ptr<UnitAction> &e) {
			return e->allow_interupt();
		});
}

UnitReference Unit::get_ref() {
	return UnitReference(&container, id, this);
}

UnitContainer *Unit::get_container() const {
	return &container;
}

std::vector<UnitAction *> Unit::current_actions() const {
	std::vector<UnitAction *> result;
	return result;
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

} /* namespace openage */
