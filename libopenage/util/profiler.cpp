// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "profiler.h"
#include "../engine.h"

#include <chrono>
#include <epoxy/gl.h>
#include <iostream>

namespace openage {
namespace util {

Profiler::~Profiler() {
	this->unregister_all();
}

void Profiler::register_component(std::string com, color component_color) {
	if (this->registered(com)) {
		return;
	}

	component_time_data cdt = component_time_data();
	cdt.display_name = com;
	cdt.drawing_color = component_color;

	for (auto it = cdt.history.begin(); it != cdt.history.end(); ++it) {
		*it = 0;
	}

	this->components[com] = cdt;
}

void Profiler::unregister_component(std::string com) {
	if (not this->registered(com)) {
		return;
	}

	this->components.erase(com);
}

void Profiler::unregister_all() {
	std::vector<std::string> registered_components = this->registered_components();

	for (auto com : registered_components) {
		this->unregister_component(com);
	}
}

std::vector<std::string> Profiler::registered_components() {
	std::vector<std::string> registered_components;
	for (auto it = this->components.begin(); it != this->components.end(); ++it) {
		registered_components.push_back(it->first);
	}

	return registered_components;
}

void Profiler::start_measure(std::string com, color component_color) {
	if (not this->registered(com)) {
		this->register_component(com, component_color);
	}

	this->components[com].start = std::chrono::high_resolution_clock::now();
}

void Profiler::end_measure(std::string com) {
	if (this->registered(com)) {
		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		this->components[com].duration = end - this->components[com].start;
	}
}

long Profiler::last_duration(std::string cat) {

	if (cat == "abs") {
		std::chrono::high_resolution_clock::duration dur = this->frame_duration;
		return std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
	}

	std::chrono::high_resolution_clock::duration dur = this->components[cat].duration;
	return std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
}

void Profiler::draw_component_performance(std::string com) {
	double percentage = this->duration_to_percentage(this->components[com].duration);
	this->append_to_history(com, percentage);

	color rgb = this->components[com].drawing_color;
	glColor4f(rgb.r, rgb.g, rgb.b, 1.0);

	glLineWidth(1.0);
	glBegin(GL_LINE_STRIP);
	float x_offset = 0.0;
	float offset_factor = (float)PROFILER_CANVAS_WIDTH / (float)MAX_DURATION_HISTORY;
	float percentage_factor = (float)PROFILER_CANVAS_HEIGHT / 100.0;

	int read_start = keep_in_duration_bound(this->insert_pos);

	for (auto i = read_start; i != keep_in_duration_bound(this->insert_pos-1); ++i) {
		i = keep_in_duration_bound(i);

		auto percentage = this->components[com].history.at(i);
		glVertex3f(PROFILER_CANVAS_POSITION_X + x_offset, PROFILER_CANVAS_POSITION_Y + percentage * percentage_factor, 0.0);
		x_offset += offset_factor;
	}
	glEnd();

	// reset color
	glColor4f(1.0, 1.0, 1.0, 1.0);
}

void Profiler::show(bool debug_mode) {
	if (debug_mode) {
		this->show();
	}
}

void Profiler::show() {

	this->draw_canvas();
	this->draw_legend();

	for (auto com : this->components) {
		this->draw_component_performance(com.first);
	}
}

bool Profiler::registered(std::string com) const {
	return this->components.find(com) != this->components.end();
}

unsigned Profiler::size() const {
	return this->components.size();
}

void Profiler::start_frame_measure() {
	this->frame_start = std::chrono::high_resolution_clock::now();
}

void Profiler::end_frame_measure() {
	auto frame_end = std::chrono::high_resolution_clock::now();
	this->frame_duration = frame_end - this->frame_start;
}

void Profiler::draw_canvas() {
	glColor4f(0.2, 0.2, 0.2, PROFILER_CANVAS_ALPHA);
	glRecti(PROFILER_CANVAS_POSITION_X,
			PROFILER_CANVAS_POSITION_Y,
			PROFILER_CANVAS_POSITION_X + PROFILER_CANVAS_WIDTH,
			PROFILER_CANVAS_POSITION_Y + PROFILER_CANVAS_HEIGHT);
}

void Profiler::draw_legend() {
	int offset = 0;
	for (auto com : this->components) {
		glColor4f(com.second.drawing_color.r, com.second.drawing_color.g, com.second.drawing_color.b, 1.0);
		int box_x = PROFILER_CANVAS_POSITION_X + 2;
		int box_y = PROFILER_CANVAS_POSITION_Y - PROFILER_COM_BOX_HEIGHT - 2 - offset;
		glRecti(box_x, box_y, box_x + PROFILER_COM_BOX_WIDTH, box_y + PROFILER_COM_BOX_HEIGHT);

		glColor4f(0.2, 0.2, 0.2, 1);
		coord::window position = coord::window();
		position.x = box_x + PROFILER_COM_BOX_WIDTH + 2;
		position.y = box_y + 2;
		Engine::get().render_text(position, 12, "%s", com.second.display_name.c_str());

		offset += PROFILER_COM_BOX_HEIGHT + 2;
	}
}

double Profiler::duration_to_percentage(std::chrono::high_resolution_clock::duration duration) {
	double dur = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	double ref = std::chrono::duration_cast<std::chrono::microseconds>(this->frame_duration).count();
	double percentage = dur / ref * 100;
	if (percentage > 100) {
		std::cout << "percentage: " << percentage << std::endl
				  << "dur: " << (double)std::chrono::duration_cast<std::chrono::microseconds>(duration).count() << std::endl
				  << "ref: " << (double)std::chrono::duration_cast<std::chrono::microseconds>(this->frame_duration).count() << std::endl;
	}
	return percentage;
}

void Profiler::append_to_history(std::string com, double percentage) {
	if (this->insert_pos == MAX_DURATION_HISTORY) {
		this->insert_pos = 0;
	}
	this->components[com].history[this->insert_pos] = percentage;
	this->insert_pos++;
}

int Profiler::keep_in_duration_bound(int value) {
	//return (value >= MAX_DURATION_HISTORY) ? 0 : value;
	return value % MAX_DURATION_HISTORY;
}

} //namespace util
} //namespace openage
