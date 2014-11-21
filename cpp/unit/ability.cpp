// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "../terrain/terrain_object.h"
#include "ability.h"
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

std::shared_ptr<UnitAction> MoveAbility::target(Unit *to_modify, coord::phys3 target) {
	return std::make_shared<MoveAction>(to_modify, this->tex, this->sound, target);
}

std::shared_ptr<UnitAction> MoveAbility::target(Unit *to_modify, Unit *target) {
	coord::phys_t radius = (1 << (path::neigh_shift + 1)) + to_modify->location->min_axis() / 2;
	return std::make_shared<MoveAction>(to_modify, this->tex, nullptr, target->get_ref(), radius);
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

std::shared_ptr<UnitAction> GatherAbility::target(Unit *, coord::phys3) {
	return nullptr;
}

std::shared_ptr<UnitAction> GatherAbility::target(Unit *to_modify, Unit *target) {
	return std::make_shared<GatherAction>(to_modify, target->get_ref(), this->tex, this->sound);
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

std::shared_ptr<UnitAction> AttackAbility::target(Unit *, coord::phys3) {
	return nullptr;
}

std::shared_ptr<UnitAction> AttackAbility::target(Unit *to_modify, Unit *target) {
	return std::make_shared<AttackAction>(to_modify, target->get_ref(), this->tex, this->sound);
}

} /* namespace openage */
