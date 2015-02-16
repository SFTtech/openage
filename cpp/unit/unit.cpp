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

Unit::Unit(UnitContainer *c, id_t id)
	:
	id{id},
	selected{false},
	location{nullptr},
	pop_destructables{false},
	container{c} {

}

Unit::~Unit() {
	// make sure terrain references are removed
	if (this->location) {
		delete this->location;
	}
}

void Unit::reset() {
	this->ability_available.clear();
	this->action_stack.clear();
	this->attribute_map.clear();
	this->pop_destructables = false;
}

bool Unit::has_action() {
	return !this->action_stack.empty();
}

UnitAction *Unit::top() {
	if (this->action_stack.empty()) {
		throw util::Error("Unit stack empty - no top action exists");
	}
	return this->action_stack.back().get();
}

bool Unit::update() {
	// if unit is not on the map then do nothing
	if (!this->location) {
		return true;
	}

	if (this->pop_destructables) {
		auto position_it = std::find_if(
			std::begin(this->action_stack),
			std::end(this->action_stack),
			[](std::unique_ptr<UnitAction> &e) {
				return e->allow_destruction();
			});
		this->action_stack.erase(position_it, std::end(this->action_stack));
	}

	/*
	 * the active action is on top
	 */
	if (this->has_action()) {
		Engine &engine = Engine::get();
		this->action_stack.back()->update(engine.lastframe_msec());

		/*
		 * check completion of all actions,
		 * pop completed actions and anything above
		 */
		auto position_it = std::find_if(
			std::begin(this->action_stack),
			std::end(this->action_stack),
			[](std::unique_ptr<UnitAction> &e) {
				return e->completed();
			});
		this->action_stack.erase(position_it, std::end(this->action_stack));
	}
	return true;
}

void Unit::draw() {
	auto top_action = this->top();
	auto &draw_pos = this->location->pos.draw;
	auto draw_graphic = top_action->type();
	if (this->graphics->count(draw_graphic) == 0) {
		log::dbg("graphic not available");
		return;
	}

	// the texture to draw with
	auto draw_texture = this->graphics->at(draw_graphic);
	if (!draw_texture) {
		log::dbg("graphic null");
		return;
	}

	// frame specified by the current action
	auto draw_frame = top_action->current_frame();

	// players color if available
	unsigned color = 0;
	if (this->has_attribute(attr_type::color)) {
		auto &c_attr = this->get_attribute<attr_type::color>();
		color = c_attr.color;
	}

	// check if object has a direction
	if (this->has_attribute(attr_type::direction)) {

		// directional textures
		auto &d_attr = this->get_attribute<attr_type::direction>();
		coord::phys3_delta draw_dir = d_attr.unit_dir;
		draw_texture->draw(draw_pos.to_camgame(), draw_dir, draw_frame, color);

		if (this->graphics->count(graphic_type::shadow) > 0) {
			auto unit_shadow = this->graphics->at(graphic_type::shadow);
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
	// unit being deleted -- can no longer control
	if (not force && 
	    (this->has_action() && 
	    not this->action_stack.back()->allow_destruction())) {
		return;
	}
	this->action_stack.push_back(std::move(action));
}

void Unit::add_attribute(AttributeContainer *attr) {
	this->attribute_map.insert(attr_map_t::value_type(attr->type, attr));
}

bool Unit::has_attribute(attr_type type) {
	return (this->attribute_map.count(type) > 0);
}

bool Unit::invoke(const Command &cmd, bool direct_command) {
	if (direct_command) {

		// drop other actions
		this->stop_actions(); 
	}

	// find suitable ability for this target
	for (auto &action : this->ability_available) {
		if (cmd.ability()[action.first] && action.second->can_invoke(*this, cmd)) {
			
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
	auto position_it = std::find_if(
		std::begin(this->action_stack),
		std::end(this->action_stack),
		[](std::unique_ptr<UnitAction> &e) {
			return e->allow_interupt();
		});
	this->action_stack.erase(position_it, std::end(this->action_stack));
}

UnitReference Unit::get_ref() {
	return UnitReference(container, id, this);
}

UnitContainer *Unit::get_container() {
	return container;
}

} /* namespace openage */
