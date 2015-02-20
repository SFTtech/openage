// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOGGING_FILESINK_H_
#define OPENAGE_LOGGING_FILESINK_H_

#include <iostream>
#include <fstream>

#include "logging.h"

namespace openage {
namespace logging {

class FileSink : public LogSink {
public:
	FileSink(const char *filename, bool append);

private:
	virtual void output_log_message(const Message &msg, Logger *source);

	std::ofstream outfile;
};

}} // namespace openage::logging

#endif
