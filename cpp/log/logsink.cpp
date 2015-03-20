// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <mutex>

#include "logsink.h"

namespace openage {
namespace log {


LogSink::LogSink() {
	std::lock_guard<std::mutex> lock(LogSink::log_sink_list_mutex);

	LogSink::log_sink_list.push_back(this);

	this->loglevel = level::dbg;
}


LogSink::~LogSink() {
	// TODO: de-constructing log_sink_list takes O(n^2) time...
	// while this is utterly insignificant, building a map upon
	// start-of-deinitialization might be prettier.

	std::lock_guard<std::mutex> lock(LogSink::log_sink_list_mutex);

	for (size_t i = 0; i < LogSink::log_sink_list.size(); i++) {
		if (LogSink::log_sink_list[i] == this) {
			// Replace the element with the last element on the vector.
			LogSink::log_sink_list[i] = LogSink::log_sink_list[LogSink::log_sink_list.size() - 1];
			// Delete the last element on the vector.
			LogSink::log_sink_list.pop_back();

			return;
		}
	}

	// We didn't find the source in the global sink list.

	// It should have been there!

	// Seriously, I'm freaked out!

	// TODO: Log the logger error!
}


std::mutex LogSink::log_sink_list_mutex;
std::vector<LogSink *> LogSink::log_sink_list;


}} // namespace openage::log
