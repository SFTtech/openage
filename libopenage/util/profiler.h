// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_PROFILER_H_
#define OPENAGE_UTIL_PROFILER_H_

#include <array>
#include <chrono>
#include <unordered_map>
#include <vector>

constexpr int MAX_DURATION_HISTORY = 100;
constexpr int PROFILER_CANVAS_WIDTH = 250;
constexpr int PROFILER_CANVAS_HEIGHT = 120;
constexpr int PROFILER_CANVAS_POSITION_X = 0;
constexpr int PROFILER_CANVAS_POSITION_Y = 300;
constexpr float PROFILER_CANVAS_ALPHA = 0.6;
constexpr int PROFILER_COM_BOX_WIDTH = 30;
constexpr int PROFILER_COM_BOX_HEIGHT = 15;

namespace openage {
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
	Profiler() = default;
	~Profiler();

	/**
	 * registers a component
	 * @param com the identifier to distinguish the components
	 * @param component_color color of the plotted line
	 */
	void register_component(std::string com, color component_color);

	/**
	 * unregisters an individual component
	 * @param com component name which should be unregistered
	 */
	void unregister_component(std::string com);

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
	 * registered, the profiler uses the color information given by
	 * component_color. The default value is white.
	 */
	void start_measure(std::string com, color component_color = {1.0, 1.0, 1.0});

	/*
	 * stops the measurement for the component com. If com is not yet
	 * registered it does nothing.
	 */
	void end_measure(std::string com);

	/*
	 * depreciated
	 * TODO remove me when profiler is finished
	 */
	long last_duration(std::string com);

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
	bool registered(std::string com) const;

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
	 * of the components
	 */
	void end_frame_measure();

private:
	std::chrono::high_resolution_clock::time_point frame_start;
	std::chrono::high_resolution_clock::duration frame_duration;
	std::unordered_map<std::string, component_time_data> components;
	int insert_pos = 0;

	void draw_canvas();
	void draw_legend();
	int keep_in_duration_bound(int value);
	void draw_component_performance(std::string com);
	double duration_to_percentage(std::chrono::high_resolution_clock::duration duration);
	void append_to_history(std::string com, double percentage);
};

} //namespace util
} //namespace openage

#endif
