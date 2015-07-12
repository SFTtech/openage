// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "logsink.h"


namespace openage {
namespace log {


LogSink::LogSink() {
	std::lock_guard<std::mutex> lock(sink_list_mutex);
	sink_list().push_back(this);

	this->loglevel = lvl::dbg;
}


LogSink::~LogSink() {
	// TODO: de-constructing log_sink_list takes O(n^2) time...
	// while this is utterly insignificant, building a map upon
	// start-of-deinitialization might be prettier.

	std::lock_guard<std::mutex> lock(sink_list_mutex);
	auto &sinks = sink_list();

	for (size_t i = 0; i < sinks.size(); i++) {
		if (sinks[i] == this) {
			// Replace the element with the last element on the vector.
			sinks[i] = sinks[sinks.size() - 1];
			// Delete the last element on the vector.
			sinks.pop_back();

			return;
		}
	}

	// We didn't find the source in the global sink list.
	// It should have been there!
	// Seriously, I'm freaked out!
	std::cout << "Logger error: Couldn't find log sink in global sink list." << std::endl;
}


std::mutex sink_list_mutex;
std::vector<LogSink *> &sink_list() {
	static std::vector<LogSink *> value;
	return value;
}


}} // namespace openage::log
