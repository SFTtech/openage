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
	auto end = std::chrono::high_resolution_clock::now();
	this->components[com].duration = end - this->components[com].start;
}

/**
 * Just for debugging
 */
long Profiler::last_duration(std::string cat) {
	auto dur = this->components[cat].duration;
	return std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
}

void Profiler::show(std::string com) {
	coord::window camgame_window = Engine::get_coord_data()->camgame_window;
	coord::window zero;
	zero.x = -camgame_window.x + PROFILER_CANVAS_POSITION_X;
	zero.y = -camgame_window.y + PROFILER_CANVAS_POSITION_Y;

	float percentage = this->duration_to_percentage(this->components[com].duration);
	this->append_percentage_to_history(com, percentage);

	color rgb = this->components[com].drawing_color;
	glColor4f(rgb.r, rgb.g, rgb.b, 1.0);

	glBegin(GL_LINE_STRIP);
	float x_offset = 0.0;
	float offset_factor = (float)PROFILER_CANVAS_WIDTH / (float)MAX_DURATION_HISTORY;
	float percentage_factor = (float)PROFILER_CANVAS_HEIGHT / 100.0;
	for (float percentage : this->components[com].history) {
		glVertex3f(zero.x + x_offset, zero.y + percentage * percentage_factor, 0.0);
		x_offset += offset_factor;
	}
	std::cout << "percentage_factor: " << percentage_factor << std::endl;
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
		this->show(com.first);
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
	coord::window camgame_window = Engine::get_coord_data()->camgame_window;

	glColor4f(0.2, 0.2, 0.2, PROFILER_CANVAS_ALPHA);
	glRecti(-camgame_window.x + PROFILER_CANVAS_POSITION_X,
			-camgame_window.y + PROFILER_CANVAS_POSITION_Y,
			-camgame_window.x + PROFILER_CANVAS_POSITION_X + PROFILER_CANVAS_WIDTH,
			-camgame_window.y + PROFILER_CANVAS_POSITION_Y + PROFILER_CANVAS_HEIGHT);
}

void Profiler::draw_legend() {
	coord::window camgame_window = Engine::get_coord_data()->camgame_window;

	int offset = 0;
	for (auto com : this->components) {
		glColor4f(com.second.drawing_color.r, com.second.drawing_color.g, com.second.drawing_color.b, 1.0);
		int box_x = -camgame_window.x + PROFILER_CANVAS_POSITION_X + 2;
		int box_y = -camgame_window.y + PROFILER_CANVAS_POSITION_Y - PROFILER_COM_BOX_HEIGHT - 2 - offset;
		glRecti(box_x, box_y, box_x + PROFILER_COM_BOX_WIDTH, box_y + PROFILER_COM_BOX_HEIGHT);

		glColor4f(0.2, 0.2, 0.2, 1);
		coord::window position = coord::window();
		position.x = box_x + PROFILER_COM_BOX_WIDTH + 2;
		position.y = box_y + 2;
		Engine::get().render_text(position, 12, com.second.display_name.c_str());

		offset += PROFILER_COM_BOX_HEIGHT + 2;
	}
}

float Profiler::duration_to_percentage(std::chrono::high_resolution_clock::duration duration) {
	auto reference_value = this->frame_duration;
	//auto percentage = duration / reference_value;
	float dur = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	float ref = std::chrono::duration_cast<std::chrono::microseconds>(this->frame_duration).count();
	float percentage = dur / ref * 100;

	std::cout << "Reference: " << std::chrono::duration_cast<std::chrono::microseconds>(this->frame_duration).count()
			  << ", measured: " << std::chrono::duration_cast<std::chrono::microseconds>(duration).count()
			  << ", percentage: " << percentage << std::endl;

	return percentage;
}

void Profiler::append_percentage_to_history(std::string com, float percentage) {
	if (this->insert_pos == MAX_DURATION_HISTORY) {
		this->insert_pos = 0;
	}
	this->components[com].history[this->insert_pos] = percentage;
	this->insert_pos++;
}

} //namespace util
} //namespace openage

