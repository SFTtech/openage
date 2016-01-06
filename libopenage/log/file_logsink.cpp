// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "file_logsink.h"

#include <iostream>
#include <iomanip>

#include "message.h"
#include "logsource.h"

namespace openage {
namespace log {


FileSink::FileSink(const char *filename, bool append)
	:
	outfile{filename, std::ios_base::out | append ? std::ios_base::app : std::ios_base::trunc} {}


void FileSink::output_log_message(const message &msg, LogSource *source) {
	this->outfile << msg.lvl->name << "|";
	this->outfile << source->logsource_name() << "|";
	this->outfile << msg.filename << ":" << msg.lineno << "|";
	this->outfile << msg.functionname << "|";
	this->outfile << msg.thread_id << "|";
	this->outfile << std::setprecision(7) << std::fixed << msg.timestamp / 1e9 << "|";
	this->outfile << msg.text << std::endl;
}

}} // namespace openage::log
