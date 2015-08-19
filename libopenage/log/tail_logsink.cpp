// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "tail_logsink.h"

#include <sstream>
#include <cstring>
#include <iostream>

#include "message.h"
#include "logsource.h"

namespace openage {
namespace log {


void TailSink::output_log_message(const message &msg, LogSource *source) {
	// write log to str
	// (mainly copied from StdOutSink).
	std::stringstream ss;
	ss << msg.lvl->name << "|";
	ss << source->logsource_name() << "|";
	ss << msg.filename << ":" << msg.lineno << "|";
	ss << "T" << msg.thread_id << "|";
	ss << std::setprecision(7) << std::fixed << msg.timestamp / 1e9 << "|";
	ss << msg.text << std::endl;
	std::string str = ss.str();

	if (tail.size() > tail_size) {
		tail.pop_front();
	}
	tail.push_back(str);
}


}} // namespace openage::log
