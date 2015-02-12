// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include <algorithm>
#include <cmath>

#include "../terrain/terrain.h"
#include "../engine.h"
#include "../crossplatform/math_constants.h"
#include "ability.h"
#include "action.h"
#include "producer.h"
#include "unit.h"

namespace openage {

Unit::Unit(UnitContainer *c, id_t id)
	:
	id{id},
	location{nullptr},
	pop_destructables{false},
	container{c} {

}

Unit::~Unit() {}

bool Unit::has_action() {
	return !this->action_stack.empty();
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

bool Unit::draw() {
	// dont draw if theres no actions, or unit is not on the map
	if (this->action_stack.empty() || !this->location) return true;
	this->action_stack.back()->draw();
	return true;
}

void Unit::give_ability(std::unique_ptr<UnitAbility> ability) {
	this->ability_available.push_back( std::move(ability) );
}

UnitAbility *Unit::get_ability(ability_type type) {
	auto position_it = std::find_if(std::begin(this->ability_available),
	                                std::end(this->ability_available),
			[&](std::unique_ptr<UnitAbility> &e) {
				return e->type() == type;
			});
	if (position_it != this->ability_available.end()) {
		return position_it->get();
	}
	return nullptr;
}

void Unit::push_action(std::unique_ptr<UnitAction> action) {
	this->action_stack.push_back(std::move(action));
}

void Unit::add_attribute(AttributeContainer *attr) {
	this->attribute_map.insert(attr_map_t::value_type(attr->type, attr));
}

bool Unit::has_attribute(attr_type type) {
	return (this->attribute_map.count(type) > 0);
}

bool Unit::target(coord::phys3 target, ability_set type) {
	// find suitable ability for this target
	for (auto &action : this->ability_available) {
		if (type[action->type()] && action->can_target(this, target)) {
			erase_interuptables();
			this->push_action( action->target(this, target) );
			return true;
		}
	}
	return false;
}

bool Unit::target(Unit *target, ability_set type) {
	// find suitable ability for this target
	for (auto &action : this->ability_available) {
		if (type[action->type()] && action->can_target(this, target)) {
			erase_interuptables();
			this->push_action( action->target(this, target) );
			return true;
		}
	}
	return false;
}

void Unit::delete_unit() {
	pop_destructables = true;
}

void Unit::erase_interuptables() {
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

unsigned int dir_group(coord::phys3_delta dir, unsigned int angles, unsigned int first_angle) {
	// normalise dir
	double len = std::hypot(dir.ne, dir.se);
	double dir_ne = static_cast<double>(dir.ne) / len;
	double dir_se = static_cast<double>(dir.se) / len;

	// formula to find the correct angle...
	return static_cast<unsigned int>(
		round(angles * atan2(dir_se, dir_ne) * (math::INV_PI / 2))
		+ first_angle
	) % angles;
}

} /* namespace openage */
