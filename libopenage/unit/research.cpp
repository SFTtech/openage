// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "../gamestate/player.h"
#include "research.h"


namespace openage {

ResearchType::ResearchType(Player &owner)
	:
	owner{owner} {
}

std::shared_ptr<Research> ResearchType::initialise() const {
	return std::make_shared<Research>(this);
}

Research::Research(const ResearchType *type)
	:
	type{type},
	active_count{0},
	completed_count{0} {
}

void Research::started() {
	this->active_count += 1;
}

void Research::stopped() {
	this->active_count -= 1;
}

void Research::completed() {
	this->active_count -= 1;
	this->completed_count += 1;
}

bool Research::can_start() const {
	return this->active_count + this->completed_count < this->type->get_max_repeats();
}

bool Research::is_researched() const {
	return this->completed_count == this->type->get_max_repeats();
}

void Research::apply() {
	// apply the patch
	this->type->apply();

	// perform category based actions
	if (type->category() == research_category::age_advance) {
		type->owner.advance_age();

	} else if (type->category() == research_category::age_advance) {
		// TODO implemnt a way to handle this category
	}
}

} // namespace openage
