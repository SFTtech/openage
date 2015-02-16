// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "../terrain/terrain_object.h"
#include "../game_main.h"
#include "../util/unique.h"
#include "ability.h"
#include "action.h"
#include "command.h"
#include "unit.h"

namespace openage {

MoveAbility::MoveAbility(Sound *s)
	:
	sound{s} {
}

bool MoveAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (cmd.has_unit()) {
		return &to_modify != cmd.unit(); // cannot target self
	}
	if (cmd.has_position()) {
		return true;
	}
	return false; 
}

void MoveAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	if (play_sound && this->sound) {
		this->sound->play();
	}

	if (cmd.has_unit()) {
		auto target = cmd.unit();

		// distance from the targets edge that is required to stop moving
		coord::phys_t radius = path::path_grid_size + (to_modify.location->min_axis() / 2);
		if (to_modify.has_attribute(attr_type::attack)) {
			auto &att = to_modify.get_attribute<attr_type::attack>();
			radius += att.range;
		}
		if (target->has_attribute(attr_type::speed)) {
			auto &sp = target->get_attribute<attr_type::speed>();
			radius += 8 * sp.unit_speed;
		}

		to_modify.push_action(util::make_unique<MoveAction>(&to_modify, target->get_ref(), radius));
	}
	else if (cmd.has_position()) {
		auto target = cmd.position();
		to_modify.push_action(util::make_unique<MoveAction>(&to_modify, target));
	}
}

TrainAbility::TrainAbility(Sound *s)
	:
	sound{s} {
}

bool TrainAbility::can_invoke(Unit &, const Command &cmd) {
	return cmd.has_producer();
}

void TrainAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	log::dbg("invoke train action");
	if (play_sound && this->sound) {
		this->sound->play();
	}
	to_modify.push_action(util::make_unique<TrainAction>(&to_modify, cmd.producer()));
}

BuildAbility::BuildAbility(Sound *s)
	:
	sound{s} {
}

bool BuildAbility::can_invoke(Unit &, const Command &cmd) {
	if (cmd.has_producer() && cmd.has_position()) {
		return true;
	}
	if (cmd.has_unit()) {
		return cmd.unit()->has_attribute(attr_type::building);
	}
	return false;
}

void BuildAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	log::dbg("invoke build action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	if (cmd.has_unit()) {
		to_modify.push_action(util::make_unique<BuildAction>(&to_modify, cmd.unit()));
	}
	else {
		to_modify.push_action(util::make_unique<BuildAction>(&to_modify, cmd.producer(), cmd.position()));
	}
}

GatherAbility::GatherAbility(Sound *s)
	:
	sound{s} {
}

bool GatherAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (cmd.has_unit()) {
		Unit *target = cmd.unit();
		return &to_modify != target &&
		       to_modify.has_attribute(attr_type::gatherer) &&
		       target->has_attribute(attr_type::resource);
	}
	return false;
}

void GatherAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	log::dbg("invoke gather action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	Unit *target = cmd.unit();
	to_modify.push_action(util::make_unique<GatherAction>(&to_modify, target->get_ref()));
}

AttackAbility::AttackAbility(Sound *s)
	:
	sound{s} {
}

bool AttackAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (cmd.has_unit()) {
		Unit *target = cmd.unit();
		return &to_modify != target && 
		       to_modify.has_attribute(attr_type::attack) &&
		       target->has_attribute(attr_type::hitpoints) &&
		       target->get_attribute<attr_type::hitpoints>().current > 0;
	}
	return false;
}

void AttackAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	if (play_sound && this->sound) {
		this->sound->play();
	}

	Unit *target = cmd.unit();
	to_modify.push_action(util::make_unique<AttackAction>(&to_modify, target->get_ref()));
}

} /* namespace openage */
