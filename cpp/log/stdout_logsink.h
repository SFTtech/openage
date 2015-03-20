// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_STDOUT_LOGSINK_H_
#define OPENAGE_LOG_STDOUT_LOGSINK_H_

#include "logsink.h"

namespace openage {
namespace log {


/**
 * Simple logsink that prints messages to stdout (via std::cout).
 */
class StdOutSink : public LogSink {
	virtual void output_log_message(const message &msg, LogSource *source);
};


/**
 * Returns a reference to the global stdout logsink object;
 * Initializes the object if needed.
 */
StdOutSink &global_stdoutsink();


}} // namespace openage::log

#endif
