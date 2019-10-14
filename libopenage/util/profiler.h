// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <array>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <string>

constexpr int MAX_DURATION_HISTORY = 100;
constexpr int PROFILER_CANVAS_WIDTH = 250;
constexpr int PROFILER_CANVAS_HEIGHT = 120;
constexpr int PROFILER_CANVAS_POSITION_X = 0;
constexpr int PROFILER_CANVAS_POSITION_Y = 300;
constexpr float PROFILER_CANVAS_ALPHA = 0.6f;
constexpr int PROFILER_COM_BOX_WIDTH = 30;
constexpr int PROFILER_COM_BOX_HEIGHT = 15;

namespace openage {

class Engine;


namespace util {

struct color {
	float r, g, b;
};

struct component_time_data {
	std::string display_name;
	color drawing_color;
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::duration duration;
	std::array<double, MAX_DURATION_HISTORY> history;
};

class Profiler {
public:
	Profiler(Engine *engine);
	~Profiler();

	/**
	 * registers a component
	 * @param com the identifier to distinguish the components
	 * @param component_color color of the plotted line
	 */
	void register_component(const std::string &com, color component_color);

	/**
	 * unregisters an individual component
	 * @param com component name which should be unregistered
	 */
	void unregister_component(const std::string &com);

	/**
	 * unregisters all remaining components
	 */
	void unregister_all();

	/**
	 *returns a vector of registered component names
	 */
	std::vector<std::string> registered_components();

	/*
	 * starts a measurement for the component com. If com is not yet
	 * registered, its getting registered and the profiler uses the color
	 * information given by component_color. The default value is white.
	 */
	void start_measure(const std::string &com, color component_color={1.0, 1.0, 1.0});

	/*
	 * stops the measurement for the component com. If com is not yet
	 * registered it does nothing.
	 */
	void end_measure(const std::string &com);

	/*
	 * draws the profiler gui if debug_mode is set
	 */
	void show(bool debug_mode);

	/*
	 * draws the profiler gui
	 */
	void show();

	/*
	 * true if the component com is already registered, otherwise false
	 */
	bool registered(const std::string &com) const;

	/*
	 * returns the number of registered components
	 */
	unsigned size() const;

	/**
	 * sets the start point for the actual frame which is used as a reference
	 * value for the registered components
	 */
	void start_frame_measure();

	/**
	 * sets the end point for the reference time used to compute the portions
	 * of the components. Each recorded measurement for the registered components
	 * get appended to their history complete the measurement.
	 */
	void end_frame_measure();

private:
	void draw_canvas();
	void draw_legend();
	void draw_component_performance(const std::string &com);
	double duration_to_percentage(std::chrono::high_resolution_clock::duration duration);
	void append_to_history(const std::string &com, double percentage);
	bool engine_in_debug_mode();

	std::chrono::high_resolution_clock::time_point frame_start;
	std::chrono::high_resolution_clock::duration frame_duration;
	std::unordered_map<std::string, component_time_data> components;
	int insert_pos = 0;

	Engine *engine;
};

}} // openage::util
