// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "../terrain/terrain_object.h"
#include "../game_main.h"
#include "../util/unique.h"
#include "ability.h"
#include "action.h"
#include "unit.h"

namespace openage {

MoveAbility::MoveAbility(Sound *s)
	:
	sound{s} {
}

bool MoveAbility::can_invoke(Unit *, uint) { return false; }
bool MoveAbility::can_invoke(Unit *, coord::phys3) { return true; }
bool MoveAbility::can_invoke(Unit *to_modify, Unit *target) { return to_modify != target; }
void MoveAbility::invoke(Unit *, uint, bool) {}

void MoveAbility::invoke(Unit *to_modify, coord::phys3 target, bool play_sound) {
	if (play_sound && this->sound) {
		this->sound->play();
	}

	to_modify->push_action(util::make_unique<MoveAction>(to_modify, target));
}

void MoveAbility::invoke(Unit *to_modify, Unit *target, bool play_sound) {
	log::dbg("invoke move action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	// distance from the targets edge that is required to stop moving
	coord::phys_t radius = path::path_grid_size + (to_modify->location->min_axis() / 2);
	if (to_modify->has_attribute(attr_type::attack)) {
		auto &att = to_modify->get_attribute<attr_type::attack>();
		radius += att.range;
	}
	if (target->has_attribute(attr_type::speed)) {
		auto &sp = target->get_attribute<attr_type::speed>();
		radius += 8 * sp.unit_speed;
	}

	to_modify->push_action(util::make_unique<MoveAction>(to_modify, target->get_ref(), radius));
}

TrainAbility::TrainAbility(Sound *s)
	:
	sound{s} {
}

bool TrainAbility::can_invoke(Unit *, uint) { return true; }
bool TrainAbility::can_invoke(Unit *, coord::phys3) { return false; }
bool TrainAbility::can_invoke(Unit *, Unit *) { return false; }

void TrainAbility::invoke(Unit *to_modify, uint, bool play_sound) {
	log::dbg("invoke train action");
	if (play_sound && this->sound) {
		this->sound->play();
	}
	to_modify->push_action(util::make_unique<TrainAction>(to_modify));
}

void TrainAbility::invoke(Unit *, coord::phys3, bool) {}
void TrainAbility::invoke(Unit *, Unit *, bool) {}

GatherAbility::GatherAbility(Sound *s)
	:
	sound{s} {
}

bool GatherAbility::can_invoke(Unit *, uint) { return false; }
bool GatherAbility::can_invoke(Unit *, coord::phys3) { return false; }

bool GatherAbility::can_invoke(Unit *to_modify, Unit *target) {
	return to_modify != target &&
	       to_modify->has_attribute(attr_type::gatherer) &&
	       target->has_attribute(attr_type::resource);
}

void GatherAbility::invoke(Unit *, uint, bool) {}
void GatherAbility::invoke(Unit *, coord::phys3, bool) {}

void GatherAbility::invoke(Unit *to_modify, Unit *target, bool play_sound) {
	log::dbg("invoke gather action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	to_modify->push_action(util::make_unique<GatherAction>(to_modify, target->get_ref()));
}

AttackAbility::AttackAbility(Sound *s)
	:
	sound{s} {
}

bool AttackAbility::can_invoke(Unit *, uint) { return false; }
bool AttackAbility::can_invoke(Unit *, coord::phys3) { return false; }

bool AttackAbility::can_invoke(Unit *to_modify, Unit *target) {
	return to_modify != target && 
	       to_modify->has_attribute(attr_type::attack) &&
	       target->has_attribute(attr_type::hitpoints) &&
	       target->get_attribute<attr_type::hitpoints>().current > 0;
}

void AttackAbility::invoke(Unit *, uint, bool) {}
void AttackAbility::invoke(Unit *, coord::phys3, bool) {}

void AttackAbility::invoke(Unit *to_modify, Unit *target, bool play_sound) {
	if (play_sound && this->sound) {
		this->sound->play();
	}	
	to_modify->push_action(util::make_unique<AttackAction>(to_modify, target->get_ref()));
}

ProjectileAbility::ProjectileAbility() {}

bool ProjectileAbility::can_invoke(Unit *, uint) { return false; }
bool ProjectileAbility::can_invoke(Unit *, coord::phys3) { return true; }
bool ProjectileAbility::can_invoke(Unit *, Unit *) { return false; }
void ProjectileAbility::invoke(Unit *, uint, bool) {}

void ProjectileAbility::invoke(Unit *to_modify, coord::phys3 target, bool) {

	// force projectile actions since they are important
	to_modify->push_action(util::make_unique<ProjectileAction>(to_modify, target), true);
}

void ProjectileAbility::invoke(Unit *, Unit *, bool) {}

} /* namespace openage */
