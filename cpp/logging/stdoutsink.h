// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOGGING_STDOUTSINK_H_
#define OPENAGE_LOGGING_STDOUTSINK_H_

#include "logging.h"

namespace openage {
namespace logging {

class StdOutSink : public LogSink {
	virtual void output_log_message(const Message &msg, Logger *source);
};

}} // namespace openage::logging

#endif
