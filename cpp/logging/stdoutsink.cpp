// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "stdoutsink.h"

#include <iostream>
#include <iomanip>

namespace openage {
namespace logging {

void StdOutSink::output_log_message(const Message &msg, Logger *source) {
	// TODO add some sort of sophisticated filter to ignore duplicates
	//      or maybe that should be part of the regular TODO filter chains?

	level_properties props = get_level_properties(msg.lvl);

	// print log level (width 4)
	std::cout << "\x1b[" << props.colorcode << "m" << std::setw(4) << props.name << "\x1b[m" " ";

	if (msg.thread_id != 0) {
		std::cout << "\x1b[32m" "thread " << msg.thread_id << "\x1b[m";
	}

	std::cout << msg << ", logged by " << source->logger_name() << std::endl;
}

}} // namespace openage::logging
