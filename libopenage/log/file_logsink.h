// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <fstream>

#include "logsink.h"

namespace openage {
namespace log {
class LogSource;
struct message;

class FileSink : public LogSink {
public:
	FileSink(const char *filename, bool append);

private:
	virtual void output_log_message(const message &msg, LogSource *source) override;

	std::ofstream outfile;
};

} // namespace log
} // namespace openage
