// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "stdout_logsink.h"

#include <iostream>
#include <iomanip>

#include "log.h"

namespace openage {
namespace log {


void StdOutSink::output_log_message(const Message &msg, LogSource *source) {
	// TODO: Add some sort of sophisticated filter to ignore duplicates.
	//       Or maybe that should be part of the regular TODO filter chains?

	level_properties props = get_level_properties(msg.meta.lvl);

	// print log level (width 4)
	std::cout << "\x1b[" << props.colorcode << "m" << std::setw(4) << props.name << "\x1b[m" " ";

	if (msg.meta.thread_id != 0) {
		std::cout << "\x1b[32m" "[T" << msg.meta.thread_id << "]\x1b[m" " ";
	}

	std::cout << msg.text;

	if (source != general_log_source) {
		std::cout << " [logged by " << source->logsource_name() << "]" << std::endl;
	}

	std::cout << std::endl;
}


}} // namespace openage::log
