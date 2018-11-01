// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#include <memory>

#include "../terrain/terrain_object.h"
#include "../gamestate/old/cost.h"
#include "../gamestate/old/player.h"
#include "ability.h"
#include "action.h"
#include "command.h"
#include "research.h"
#include "unit.h"

namespace openage {

bool UnitAbility::has_hitpoints(Unit &target) {
	return target.has_attribute(attr_type::damaged) &&
	       target.get_attribute<attr_type::damaged>().hp > 0;
}

bool UnitAbility::is_damaged(Unit &target) {
	return target.has_attribute(attr_type::damaged) && target.has_attribute(attr_type::hitpoints) &&
	       target.get_attribute<attr_type::damaged>().hp < target.get_attribute<attr_type::hitpoints>().hp;
}

bool UnitAbility::has_resource(Unit &target) {
	return target.has_attribute(attr_type::resource) && !target.has_attribute(attr_type::worker) &&
	       target.get_attribute<attr_type::resource>().amount > 0;
}

bool UnitAbility::is_same_player(Unit &to_modify, Unit &target) {
	if (to_modify.has_attribute(attr_type::owner) &&
	    target.has_attribute(attr_type::owner)) {
		auto &mod_player = to_modify.get_attribute<attr_type::owner>().player;
		auto &tar_player = target.get_attribute<attr_type::owner>().player;
		return mod_player.color == tar_player.color;
	}
	return false;

}

bool UnitAbility::is_ally(Unit &to_modify, Unit &target) {
	if (to_modify.has_attribute(attr_type::owner) &&
	    target.has_attribute(attr_type::owner)) {
		auto &mod_player = to_modify.get_attribute<attr_type::owner>().player;
		auto &tar_player = target.get_attribute<attr_type::owner>().player;
		return mod_player.is_ally(tar_player);
	}
	return false;
}

bool UnitAbility::is_enemy(Unit &to_modify, Unit &target) {
	if (to_modify.has_attribute(attr_type::owner) &&
		target.has_attribute(attr_type::owner)) {
		auto &mod_player = to_modify.get_attribute<attr_type::owner>().player;
		auto &tar_player = target.get_attribute<attr_type::owner>().player;
		return mod_player.is_enemy(tar_player);
	}
	return false;
}

MoveAbility::MoveAbility(const Sound *s)
	:
	sound{s} {
}

bool MoveAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (cmd.has_position()) {
		return bool(to_modify.location);
	}
	else if (cmd.has_unit()) {
		return to_modify.location &&
		       cmd.unit()->location &&
		       &to_modify != cmd.unit(); // cannot target self
	}
	return false;
}

void MoveAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	to_modify.log(MSG(dbg) << "invoke move action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	if (cmd.has_position()) {
		auto target = cmd.position();
		to_modify.push_action(std::make_unique<MoveAction>(&to_modify, target));
	}
	else if (cmd.has_unit()) {
		auto target = cmd.unit();

		// distance from the targets edge that is required to stop moving
		coord::phys_t radius = path::path_grid_size + (to_modify.location->min_axis() / 2L);

		// add the range of the unit if cmd indicator is set
		if (cmd.has_flag(command_flag::use_range) && to_modify.has_attribute(attr_type::attack)) {
			auto &att = to_modify.get_attribute<attr_type::attack>();
			radius += att.max_range;
		}
		to_modify.push_action(std::make_unique<MoveAction>(&to_modify, target->get_ref(), radius));
	}
}

SetPointAbility::SetPointAbility() = default;

bool SetPointAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	return cmd.has_position() &&
	       to_modify.has_attribute(attr_type::building);
}

void SetPointAbility::invoke(Unit &to_modify, const Command &cmd, bool) {
	auto &build_attr = to_modify.get_attribute<attr_type::building>();
	build_attr.gather_point = cmd.position();
}


GarrisonAbility::GarrisonAbility(const Sound *s)
	:
	sound{s} {
}

bool GarrisonAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (!cmd.has_unit()) {
		return false;
	}
	Unit &target = *cmd.unit();

	// make sure buildings are completed
	if (target.has_attribute(attr_type::building)) {
		auto &build_attr = target.get_attribute<attr_type::building>();
		if (build_attr.completed < 1.0f) {
			return false;
		}
	}
	return to_modify.location &&
	       target.has_attribute(attr_type::garrison) &&
	       is_ally(to_modify, target);
}

void GarrisonAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	to_modify.log(MSG(dbg) << "invoke garrison action");
	if (play_sound && this->sound) {
		this->sound->play();
	}
	to_modify.push_action(std::make_unique<GarrisonAction>(&to_modify, cmd.unit()->get_ref()));
}

UngarrisonAbility::UngarrisonAbility(const Sound *s)
	:
	sound{s} {
}

bool UngarrisonAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (to_modify.has_attribute(attr_type::garrison)) {
		auto &garrison_attr = to_modify.get_attribute<attr_type::garrison>();
		return cmd.has_position() && !garrison_attr.content.empty();
	}
	return false;
}

void UngarrisonAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	to_modify.log(MSG(dbg) << "invoke ungarrison action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	// add as secondary, so primary action is not disrupted
	to_modify.secondary_action(std::make_unique<UngarrisonAction>(&to_modify, cmd.position()));
}

TrainAbility::TrainAbility(const Sound *s)
	:
	sound{s} {
}

bool TrainAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (to_modify.has_attribute(attr_type::building)) {
		auto &build_attr = to_modify.get_attribute<attr_type::building>();
		return cmd.has_type() && 1.0f <= build_attr.completed;
	}
	return false;
}

void TrainAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	to_modify.log(MSG(dbg) << "invoke train action");
	if (play_sound && this->sound) {
		this->sound->play();
	}
	to_modify.push_action(std::make_unique<TrainAction>(&to_modify, cmd.type()));
}

ResearchAbility::ResearchAbility(const Sound *s)
	:
	sound{s} {
}

bool ResearchAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (to_modify.has_attribute(attr_type::owner) && cmd.has_research()) {
		auto &player = to_modify.get_attribute<attr_type::owner>().player;
		auto research = cmd.research();
		return research->can_start() &&
		       player.can_deduct(research->type->get_research_cost().get(player));
	}
	return false;
}

void ResearchAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	if (play_sound && this->sound) {
		this->sound->play();
	}
	to_modify.push_action(std::make_unique<ResearchAction>(&to_modify, cmd.research()));
}

BuildAbility::BuildAbility(const Sound *s)
	:
	sound{s} {
}

bool BuildAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (cmd.has_unit()) {
		Unit *target = cmd.unit();
		return to_modify.location &&
		       is_same_player(to_modify, *target) &&
		       target->has_attribute(attr_type::building) &&
		       target->get_attribute<attr_type::building>().completed < 1.0f;
	}
	return false;
}

void BuildAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	to_modify.log(MSG(dbg) << "invoke build action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	if (cmd.has_unit()) {
		to_modify.push_action(std::make_unique<BuildAction>(&to_modify, cmd.unit()->get_ref()));
	}
}

GatherAbility::GatherAbility(const Sound *s)
	:
	sound{s} {
}

bool GatherAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (cmd.has_unit()) {
		Unit &target = *cmd.unit();
		return &to_modify != &target &&
		       to_modify.location &&
		       to_modify.has_attribute(attr_type::worker) &&
		       has_resource(target);
	}
	return false;
}

void GatherAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	to_modify.log(MSG(dbg) << "invoke gather action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	Unit *target = cmd.unit();
	try {
		to_modify.push_action(std::make_unique<GatherAction>(&to_modify, target->get_ref()));
	} catch (const std::invalid_argument &e) {
		to_modify.log(MSG(dbg) << "invoke gather action cancelled due to an exception. Reason: " << e.what());
	}
}

AttackAbility::AttackAbility(const Sound *s)
	:
	sound{s} {
}

bool AttackAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (cmd.has_unit()) {
		Unit &target = *cmd.unit();
		bool target_is_resource = has_resource(target);
		return &to_modify != &target &&
		       to_modify.location &&
		       target.location &&
		       target.location->is_placed() &&
		       to_modify.has_attribute(attr_type::attack) &&
		       has_hitpoints(target) &&
		       (is_enemy(to_modify, target) || target_is_resource) &&
		       (cmd.has_flag(command_flag::attack_res) == target_is_resource);
	}
	return false;
}

void AttackAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	to_modify.log(MSG(dbg) << "invoke attack action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	Unit *target = cmd.unit();
	to_modify.push_action(std::make_unique<AttackAction>(&to_modify, target->get_ref()));
}

RepairAbility::RepairAbility(const Sound *s)
	:
	sound{s} {
}

bool RepairAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (cmd.has_unit()) {
		Unit &target = *cmd.unit();
		return &to_modify != &target &&
		       to_modify.location &&
		       target.location &&
		       target.location->is_placed() &&
		       is_damaged(target) &&
		       is_ally(to_modify, target);
	}
	return false;
}

void RepairAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	to_modify.log(MSG(dbg) << "invoke repair action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	Unit *target = cmd.unit();
	to_modify.push_action(std::make_unique<RepairAction>(&to_modify, target->get_ref()));
}

HealAbility::HealAbility(const Sound *s)
	:
	sound{s} {
}

bool HealAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (cmd.has_unit()) {
		Unit &target = *cmd.unit();
		return &to_modify != &target &&
		       to_modify.location &&
		       target.location &&
		       target.location->is_placed() &&
		       is_damaged(target) &&
		       is_ally(to_modify, target);
	}
	return false;
}

void HealAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	to_modify.log(MSG(dbg) << "invoke heal action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	Unit *target = cmd.unit();
	to_modify.push_action(std::make_unique<HealAction>(&to_modify, target->get_ref()));
}

PatrolAbility::PatrolAbility(const Sound *s)
	:
	sound{s} {
}

bool PatrolAbility::can_invoke(Unit &/*to_modify*/, const Command &/*cmd*/) {
	// TODO implement
	return false;
}

void PatrolAbility::invoke(Unit &to_modify, const Command &/*cmd*/, bool play_sound) {
	to_modify.log(MSG(dbg) << "not implemented");
	if (play_sound && this->sound) {
		this->sound->play();
	}
	// TODO implement
}

ConvertAbility::ConvertAbility(const Sound *s)
	:
	sound{s} {
}

bool ConvertAbility::can_invoke(Unit &to_modify, const Command &cmd) {
	if (cmd.has_unit()) {
		Unit &target = *cmd.unit();
		return &to_modify != &target &&
		       to_modify.location &&
		       target.location &&
		       target.location->is_placed() &&
		       is_enemy(to_modify, target);
	}
	return false;
}

void ConvertAbility::invoke(Unit &to_modify, const Command &cmd, bool play_sound) {
	to_modify.log(MSG(dbg) << "invoke convert action");
	if (play_sound && this->sound) {
		this->sound->play();
	}

	Unit *target = cmd.unit();
	to_modify.push_action(std::make_unique<ConvertAction>(&to_modify, target->get_ref()));
}

ability_set UnitAbility::set_from_list(const std::vector<ability_type> &items) {
	ability_set result;
	for (auto i : items) {
		result[static_cast<int>(i)] = 1;
	}
	return result;
}

} // namespace openage

namespace std {

string to_string(const openage::ability_type &at) {
	switch (at) {
	case openage::ability_type::move:
		return "move";
	case openage::ability_type::garrison:
		return "garrison";
	case openage::ability_type::ungarrison:
		return "ungarrison";
	case openage::ability_type::patrol:
		return "patrol";
	case openage::ability_type::train:
		return "train";
	case openage::ability_type::build:
		return "build";
	case openage::ability_type::research:
		return "research";
	case openage::ability_type::gather:
		return "gather";
	case openage::ability_type::attack:
		return "attack";
	case openage::ability_type::convert:
		return "convert";
	case openage::ability_type::repair:
		return "repair";
	case openage::ability_type::heal:
		return "heal";
	default:
		return "unknown";
	}
}

} // namespace std
