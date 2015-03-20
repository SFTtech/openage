// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "log.h"

#include <cstdarg>
#include <iostream>
#include <iomanip>

#include "named_logsource.h"
#include "stdout_logsink.h"

namespace openage {
namespace log {


// Made available as extern through log.h; initialized during log::init().
LogSource *general_log_source = nullptr;
StdOutSink *stdout_log_sink = nullptr;


void init() {
	stdout_log_sink = new StdOutSink;
	general_log_source = new NamedLogSource{"general"};
}


void deinit() {
	delete general_log_source;
	delete stdout_log_sink;
}


void log(MessageBuilder &msg) {
	general_log_source->log(msg);
}


}} // namespace openage::log
