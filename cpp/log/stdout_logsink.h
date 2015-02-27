// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_STDOUT_LOGSINK_H_
#define OPENAGE_LOG_STDOUT_LOGSINK_H_

#include "logsink.h"

namespace openage {
namespace log {

class StdOutSink : public LogSink {
	virtual void output_log_message(const Message &msg, LogSource *source);
};

}} // namespace openage::log

#endif
