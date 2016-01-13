// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "profiler.h"
#include "../engine.h"
#include "misc.h"

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

	component_time_data cdt;
	cdt.display_name = com;
	cdt.drawing_color = component_color;

	for (auto &val : cdt.history) {
		val = 0;
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
	for (auto &pair : this->components) {
		registered_components.push_back(pair.first);
	}

	return registered_components;
}

void Profiler::start_measure(std::string com, color component_color) {
	if (not this->engine_in_debug_mode()) {
		return;
	}

	if (not this->registered(com)) {
		this->register_component(com, component_color);
	}

	this->components[com].start = std::chrono::high_resolution_clock::now();
}

void Profiler::end_measure(std::string com) {
	if (not this->engine_in_debug_mode()) {
		return;
	}

	if (this->registered(com)) {
		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		this->components[com].duration = end - this->components[com].start;
	}
}

void Profiler::draw_component_performance(std::string com) {
	color rgb = this->components[com].drawing_color;
	glColor4f(rgb.r, rgb.g, rgb.b, 1.0);

	glLineWidth(1.0);
	glBegin(GL_LINE_STRIP);
	float x_offset = 0.0;
	float offset_factor = (float)PROFILER_CANVAS_WIDTH / (float)MAX_DURATION_HISTORY;
	float percentage_factor = (float)PROFILER_CANVAS_HEIGHT / 100.0;

	for (auto i = this->insert_pos; mod(i, MAX_DURATION_HISTORY) != mod(this->insert_pos-1, MAX_DURATION_HISTORY); ++i) {
		i = mod(i, MAX_DURATION_HISTORY);

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
	if (this->engine_in_debug_mode()) {
		this->frame_start = std::chrono::high_resolution_clock::now();
	}
}

void Profiler::end_frame_measure() {
	if (not this->engine_in_debug_mode()) {
		return;
	}

	auto frame_end = std::chrono::high_resolution_clock::now();
	this->frame_duration = frame_end - this->frame_start;

	for (auto com : this->registered_components()) {
		double percentage = this->duration_to_percentage(this->components[com].duration);
		this->append_to_history(com, percentage);
	}

	this->insert_pos++;
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
	return percentage;
}

void Profiler::append_to_history(std::string com, double percentage) {
	if (this->insert_pos == MAX_DURATION_HISTORY) {
		this->insert_pos = 0;
	}
	this->components[com].history[this->insert_pos] = percentage;
}

bool Profiler::engine_in_debug_mode() {
	Engine &engine = Engine::get();
	if (engine.drawing_debug_overlay.value) {
		return true;
	} else {
		return false;
	}
}

}} // openage::util
