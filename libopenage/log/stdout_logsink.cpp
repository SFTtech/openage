// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "stdout_logsink.h"

#include <iostream>
#include <iomanip>

#include "named_logsource.h"

namespace openage {
namespace log {


void StdOutSink::output_log_message(const message &msg, LogSource *source) {
	// print log level (width 4)
	std::cout << "\x1b[" << msg.lvl->colorcode << "m" << std::setw(4) << msg.lvl->name << "\x1b[m" " ";

	if (msg.thread_id != 0) {
		std::cout << "\x1b[32m" "[T" << msg.thread_id << "]\x1b[m ";
	}

	if (source != &general_source()) {
		std::cout << "\x1b[36m" "[" << source->logsource_name() << "]\x1b[m ";
	}

	std::cout << msg.text << std::endl;
}


StdOutSink &global_stdoutsink() {
	static StdOutSink value;
	return value;
}


}} // namespace openage::log
