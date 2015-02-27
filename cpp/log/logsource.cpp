// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "logsource.h"

#include "logsink.h"

namespace openage {
namespace log {


LogSource::LogSource()
	:
	logger_id{LogSource::get_unique_logger_id()} {}


void LogSource::log(MessageBuilder &msg_builder) {
	Message &msg = msg_builder.finalize();

	std::lock_guard<std::mutex> lock(LogSink::log_sink_list_mutex);

	// By the docs of log::init, an empty log_sink_list means undefined behavior.
	// However, we're generous.
	if (LogSink::log_sink_list.size() == 0) {
		std::cout << "No log sinks are available; Dumping message stdout." << std::endl;
		std::cout << msg;
	}

	for (LogSink *sink : LogSink::log_sink_list) {
		// TODO: more sophisticated filtering (iptables-chains-like)
		if (msg.meta.lvl >= sink->loglevel) {
			sink->output_log_message(msg, this);
		}
	}
}


size_t LogSource::get_unique_logger_id() {
	// Strictly-monotonically increasing counter.
	static std::atomic<size_t> ctr{0};

	return ctr++;
}


}} // namespace openage::log
