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
	std::string display_name;
	color drawing_color;
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::duration duration;
	std::array<std::chrono::high_resolution_clock::duration, MAX_DURATION_HISTORY> history;
};

class Profiler {
public:

	Profiler() = default;
	~Profiler();
	void register_component(std::string com, color component_color);
	void unregister_component(std::string com);
	void unregister_all();
	std::vector<std::string> registered_components();

	void start_measure(std::string com, color component_color);
	void end_measure(std::string com);
	long last_duration(std::string com);
	void show(std::string com);
	void show(bool debug_mode);
	void show();
	bool registered(std::string com) const;
	unsigned size() const;

private:
	std::map<std::string, component_time_data> components;
	int insert_pos = 0;

	void draw_canvas();
	void draw_legend();
};

} //namespace util
} //namespace openage

#endif
