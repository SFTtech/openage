// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_PROFILER_H_
#define OPENAGE_UTIL_PROFILER_H_

#include <map>
#include <vector>
#include <chrono>
#include <array>
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
	int r, g, b;
};

struct component_time_data {
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::duration duration;
	std::array<std::chrono::high_resolution_clock::duration, MAX_DURATION_HISTORY> history;
	color drawing_color;
	std::string name;
};

class Profiler {
public:
	// TODO Maybe remove enum class and use strings instead
	// PROS:
	//  - no later adjustment of the component enum class
	//  - no unessessary string abbreviation parameter
	enum class component {
		IDLE_TIME,
		EVENT_PROCESSING,
		RENDERING,
	};

	Profiler() = default;
	~Profiler();
	void register_component(component com, color c, std::string abbreviation);
	void unregister_component(component com);
	void unregister_all();
	std::vector<component> registered_components();

	void start_measure(component com, color c, std::string abbreviation = "undef");
	void end_measure(component com);
	long last_duration(component com);
	void show(component com);
	void show(bool debug_mode);
	void show();
	bool registered(component com) const;
	unsigned size() const;

private:
	std::map<component, component_time_data> components;
	int insert_pos = 0;

	void draw_canvas();
	void draw_legend();
};

} //namespace util
} //namespace openage

#endif
