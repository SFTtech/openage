// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_FILE_LOGSINK_H_
#define OPENAGE_LOG_FILE_LOGSINK_H_

#include <iostream>
#include <fstream>

#include "logsink.h"

namespace openage {
namespace log {

class FileSink : public LogSink {
public:
	FileSink(const char *filename, bool append);

private:
	virtual void output_log_message(const message &msg, LogSource *source) override;

	std::ofstream outfile;
};

}} // namespace openage::log

#endif
