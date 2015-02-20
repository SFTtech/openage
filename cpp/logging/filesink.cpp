// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "filesink.h"

#include <iostream>
#include <iomanip>

namespace openage {
namespace logging {

FileSink::FileSink(const char *filename, bool append)
	:
	outfile{filename, std::ios_base::out | append ? std::ios_base::app : std::ios_base::trunc} {}

void FileSink::output_log_message(const Message &msg, Logger *source) {
	level_properties props = get_level_properties(msg.lvl);

	this->outfile << props.name << "|";
	this->outfile << source->logger_name() << "|";
	this->outfile << msg.msg_id << "|";
	this->outfile << msg.sourcefile << ":" << msg.lineno << "|";
	this->outfile << msg.functionname << "|";
	this->outfile << msg.thread_id << "|";
	this->outfile << msg.timestamp / 1e9 << "|";
	this->outfile << msg.text << std::endl;
}

}} // namespace openage::logging
