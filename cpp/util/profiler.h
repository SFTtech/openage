// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_PROFILER_H_
#define OPENAGE_UTIL_PROFILER_H_

#include <map>
#include <vector>
#include <chrono>
#include <list>

constexpr int MAX_DURATION_HISTORY = 100;

namespace openage {
namespace util {

struct component_time_data {
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::duration duration;
	std::list<std::chrono::high_resolution_clock::duration> history;
};

class Profiler {
public:
	enum class component {
		IDLE_TIME,
		EVENT_PROCESSING,
		RENDERING,
	};

	Profiler() = default;
	void register_component(component cat);
	void unregister_component(component cat);
	void unregister_all();
	std::vector<component> registered_components();

	void start_measure(component cat);
	void end_measure(component cat);
	long last_duration(component cat);
	void show(component cat);
	void show(bool debug_mode);
	void show();
	bool registered(component cat) const;
	unsigned size() const;

private:
	std::map<component, component_time_data> components;
};

} //namespace util
} //namespace openage

#endif
