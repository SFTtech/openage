// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "log.h"

#include "log/named_logsource.h"
#include "log/stdout_logsink.h"


namespace openage {
namespace log {


void log(const message &msg) {
	general_source().log(msg);
}


void set_level(level lvl) {
	global_stdoutsink().set_loglevel(lvl);
}


} // namespace log
} // namespace openage
