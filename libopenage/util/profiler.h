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

struct CategorieTimeData {
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::duration duration;
	std::list<std::chrono::high_resolution_clock::duration> history;
};

class Profiler {
public:
	enum class Categorie {
		IDLE_TIME,
		EVENT_PROCESSING,
		RENDERING,
	};

	Profiler() = default;
	void register_category(Categorie cat);
	void unregister_category(Categorie cat);
	void unregister_all();
	std::vector<Categorie> registered_categories();

	void start_measure(Categorie cat);
	void end_measure(Categorie cat);
	long last_duration(Categorie cat);
	void show(Categorie cat);
	void show(bool debug_mode);
	void show();
	bool registered(Categorie cat) const;
	unsigned size() const;

private:
	std::map<Categorie, CategorieTimeData> categories;
};

} //namespace util
} //namespace openage

#endif // OPENAGE_UTIL_PROFILER_H_
