// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_TAIL_LOGSINK_H_
#define OPENAGE_LOG_TAIL_LOGSINK_H_

#include <list>

#include "logsink.h"

namespace openage {
namespace log {


class TailSink : public LogSink {
public:
	const static unsigned int tail_size = 8;
	std::list<std::string> tail;

private:
	void output_log_message(const message &msg, LogSource *source) override;
};


}} // namespace openage::log


#endif
