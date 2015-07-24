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

//void PerformancePie::draw(float frame_time) {
//	float step = 5.0f;
//	int radius = 100;
//
//	float prev_angle = 0.0f;
//	for (auto it = this->categories.begin(); it != this->categories.end(); ++it) {
//		glBegin(GL_POLYGON);
//		glColor4f(it->second.color.r, it->second.color.g, it->second.color.b, this->alpha_value);
//		float angle = prev_angle;
//		float percentage = get_percentage(frame_time, it->second.last_measured_time);
//		for(; angle <= percentage + prev_angle; angle += step) {
//			float rad = this->degree2radian(angle);
//			float x = this->radius*sin(rad);
//			float y = this->radius*cos(rad);
//
//			glVertex3f(this->center.x + x, this->center.y + y, 0.0f);
//		}
//		glVertex3f(this->center.x, this->center.y, 0.0f);
//
//		glEnd();
//	}
//}

} //namespace util
} //namespace openage

