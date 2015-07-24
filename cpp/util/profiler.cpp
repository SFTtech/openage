// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "profiler.h"
#include <chrono>

namespace openage {
namespace util {

void Profiler::register_category(Categorie cat) {
	if (this->registered(cat)) {
		return;
	}

	this->categories[cat] = std::make_shared<CategorieTimeData>();
}

void Profiler::unregister_category(Categorie cat) {
	if (not this->registered(cat)) {
		return;
	}

	this->categories.erase(cat);
}

void Profiler::unregister_all() {
	std::vector<Categorie> registered_categories = this->registered_categories();

	for (auto cat : registered_categories) {
		this->unregister_category(cat);
	}
}

std::vector<Profiler::Categorie> Profiler::registered_categories() {
	std::vector<Categorie> registered_categories;
	for (auto it = this->categories.begin(); it != this->categories.end(); ++it) {
		registered_categories.push_back(it->first);
	}

	return registered_categories;
}

void Profiler::start_measure(Categorie cat) {
	this->categories[cat]->start = std::chrono::high_resolution_clock::now();
}

void Profiler::end_measure(Categorie cat) {
	auto end = std::chrono::high_resolution_clock::now();
	this->categories[cat]->duration = end - this->categories[cat]->start;
}

/**
 * Just for debugging
 */
long Profiler::last_duration(Categorie cat) {
	auto dur = this->categories[cat]->duration;
	return std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
}

void Profiler::show(Categorie cat) {
	// TODO do drawing stuff

	// append duration in history
	auto dur = this->categories[cat]->duration;
	this->categories[cat]->history.push_front(dur);

	if (this->categories[cat]->history.size() > MAX_DURATION_HISTORY) {
		this->categories[cat]->history.pop_back();
	}
}

void Profiler::show(bool debug_mode) {
	if (debug_mode) {
		this->show();
	}
}

void Profiler::show() {
	for (auto cat : this->categories) {
		this->show(cat.first);
	}
}

bool Profiler::registered(Categorie cat) const {
	return this->categories.find(cat) != this->categories.end();
}

unsigned Profiler::size() const {
	return this->categories.size();
}

} //namespace util
} //namespace openage

