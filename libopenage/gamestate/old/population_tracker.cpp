// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "population_tracker.h"

namespace openage {

PopulationTracker::PopulationTracker(int capacity_static, int capacity_max) {
	this->demand = 0;
	this->capacity_static = capacity_static;
	this->capacity_real = 0;
	this->capacity_max = capacity_max;
	this->update_capacity();
}

void PopulationTracker::demand_population(int i) {
	this->demand += i;
	// TODO triger gui update
}

void PopulationTracker::free_population(int i) {
	this->demand -= i;
	// TODO triger gui update
}

void PopulationTracker::add_capacity_static(int i) {
	this->capacity_static += i;
	this->update_capacity();
}

void PopulationTracker::add_capacity(int i) {
	this->capacity_real += i;
	this->update_capacity();
}

void PopulationTracker::remove_capacity(int i) {
	this->capacity_real -= i;
	this->update_capacity();
}

void PopulationTracker::add_capacity_max(int i) {
	this->capacity_max += i;
	this->update_capacity();
}

void PopulationTracker::update_capacity() {
	this->capacity_total = this->capacity_static + this->capacity_real;
	// check the capacity limit
	if (this->capacity_total > this->capacity_max) {
		this->capacity = this->capacity_max;
	} else {
		this->capacity = this->capacity_total;
	}
	// TODO triger gui update
}

int PopulationTracker::get_demand() const {
	return this->demand;
}

int PopulationTracker::get_capacity() const {
	return this->capacity;
}

int PopulationTracker::get_space() const {
	return this->capacity - this->demand;
}

int PopulationTracker::get_capacity_overflow() const {
	return this->capacity_total - this->capacity;
}

bool PopulationTracker::is_capacity_maxed() const {
	return this->capacity >= this->capacity_max;
}

} // openage
