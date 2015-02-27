// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "file_logsink.h"

#include <iostream>
#include <iomanip>

namespace openage {
namespace log {

FileSink::FileSink(const char *filename, bool append)
	:
	outfile{filename, std::ios_base::out | append ? std::ios_base::app : std::ios_base::trunc} {}

void FileSink::output_log_message(const Message &msg, LogSource *source) {
	level_properties props = get_level_properties(msg.meta.lvl);

	this->outfile << props.name << "|";
	this->outfile << source->logsource_name() << "|";
	this->outfile << msg.meta.msg_id << "|";
	this->outfile << msg.meta.sourcefile << ":" << msg.meta.lineno << "|";
	this->outfile << msg.meta.functionname << "|";
	this->outfile << msg.meta.thread_id << "|";
	this->outfile << msg.meta.timestamp / 1e9 << "|";
	this->outfile << msg.text << std::endl;
}

}} // namespace openage::log
