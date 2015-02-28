// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "ability.h"

#include <memory>

#include "../terrain/terrain_object.h"
#include "action.h"
#include "unit.h"

namespace openage {

MoveAbility::MoveAbility(Texture *t, TestSound *s)
	:
	tex{t},
	sound{s} {
}

bool MoveAbility::can_target(Unit *, coord::phys3) {
	return true;
}

bool MoveAbility::can_target(Unit *, Unit *) {
	return false;
}

std::unique_ptr<UnitAction> MoveAbility::target(Unit *to_modify, coord::phys3 target) {
	return std::make_unique<MoveAction>(to_modify, this->tex,
	                                    this->sound, target);
}

std::unique_ptr<UnitAction> MoveAbility::target(Unit *to_modify, Unit *target) {
	coord::phys_t radius = (path::path_grid_size * 2) + to_modify->location->min_axis() / 2;
	return std::make_unique<MoveAction>(to_modify, this->tex,
	                                    nullptr, target->get_ref(),
	                                    radius);
}

GatherAbility::GatherAbility(Texture *t, TestSound *s)
	:
	tex{t},
	sound{s} {
}

bool GatherAbility::can_target(Unit *, coord::phys3) {
	return false;
}

bool GatherAbility::can_target(Unit *u1, Unit *target) {
	return u1 != target;
}

std::unique_ptr<UnitAction> GatherAbility::target(Unit *, coord::phys3) {
	return nullptr;
}

std::unique_ptr<UnitAction> GatherAbility::target(Unit *to_modify, Unit *target) {
	return std::make_unique<GatherAction>(to_modify, target->get_ref(),
	                                      this->tex, this->sound);
}

AttackAbility::AttackAbility(Texture *t, TestSound *s)
	:
	tex{t},
	sound{s} {

}

bool AttackAbility::can_target(Unit *, coord::phys3) {
	return false;
}

bool AttackAbility::can_target(Unit *u1, Unit *target) {
	return u1 != target &&
	       target->has_attribute(attr_type::hitpoints) &&
	       target->get_attribute<attr_type::hitpoints>().current > 0;
}

std::unique_ptr<UnitAction> AttackAbility::target(Unit *, coord::phys3) {
	return nullptr;
}

std::unique_ptr<UnitAction> AttackAbility::target(Unit *to_modify, Unit *target) {
	return std::make_unique<AttackAction>(to_modify, target->get_ref(),
	                                      this->tex, this->sound);
}

} /* namespace openage */
