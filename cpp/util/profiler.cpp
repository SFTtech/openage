// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "profiler.h"
#include <chrono>

namespace openage {
namespace util {

void Profiler::register_component(component cat) {
	if (this->registered(cat)) {
		return;
	}

	this->components[cat] = component_time_data();
}

void Profiler::unregister_component(component cat) {
	if (not this->registered(cat)) {
		return;
	}

	this->components.erase(cat);
}

void Profiler::unregister_all() {
	std::vector<component> registered_components = this->registered_components();

	for (auto cat : registered_components) {
		this->unregister_component(cat);
	}
}

std::vector<Profiler::component> Profiler::registered_components() {
	std::vector<component> registered_components;
	for (auto it = this->components.begin(); it != this->components.end(); ++it) {
		registered_components.push_back(it->first);
	}

	return registered_components;
}

void Profiler::start_measure(component cat) {
	if (not this->registered(cat)) {
		this->register_component(cat);
	}

	this->components[cat].start = std::chrono::high_resolution_clock::now();
}

void Profiler::end_measure(component cat) {
	auto end = std::chrono::high_resolution_clock::now();
	this->components[cat].duration = end - this->components[cat].start;
}

/**
 * Just for debugging
 */
long Profiler::last_duration(component cat) {
	auto dur = this->components[cat].duration;
	return std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
}

void Profiler::show(component cat) {
	// TODO do drawing stuff

	// append duration in history
	auto dur = this->components[cat].duration;
	this->components[cat].history.push_front(dur);

	if (this->components[cat].history.size() > MAX_DURATION_HISTORY) {
		this->components[cat].history.pop_back();
	}
}

void Profiler::show(bool debug_mode) {
	if (debug_mode) {
		this->show();
	}
}

void Profiler::show() {
	for (auto cat : this->components) {
		this->show(cat.first);
	}
}

bool Profiler::registered(component cat) const {
	return this->components.find(cat) != this->components.end();
}

unsigned Profiler::size() const {
	return this->components.size();
}

} //namespace util
} //namespace openage

