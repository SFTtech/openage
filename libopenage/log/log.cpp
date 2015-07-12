// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "log.h"

#include "named_logsource.h"
#include "stdout_logsink.h"

namespace openage {
namespace log {


void log(const message &msg) {
	general_source().log(msg);
}


void set_level(level lvl) {
	global_stdoutsink().loglevel = lvl;
}


}} // namespace openage::log
