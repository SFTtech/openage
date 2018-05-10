// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "message.h"

#include <mutex>
#include <unordered_set>

#include "../util/timing.h"
#include "../util/compiler.h"
#include "../util/thread_id.h"
#include "../util/strings.h"

namespace openage {
namespace log {


void message::init() {
	this->thread_id = util::get_current_thread_id();
	this->timestamp = timing::get_real_time();
}


void message::init_with_metadata_copy(const std::string &filename, const std::string &functionname) {
	static std::unordered_set<std::string> stringconstants;
	static std::mutex stringconstants_mutex;

	std::lock_guard<std::mutex> lock{stringconstants_mutex};

	this->init();
	this->filename = stringconstants.insert(filename).first->c_str();
	this->functionname = stringconstants.insert(functionname).first->c_str();
}


MessageBuilder::MessageBuilder(const char *filename,
                               unsigned lineno,
                               const char *functionname,
                               level lvl)
	:
	StringFormatter<MessageBuilder>{msg.text} {

		this->msg.filename = filename;
		this->msg.lineno = lineno;
		this->msg.functionname = functionname;
		this->msg.lvl = lvl;

		this->msg.init();
	}


std::ostream &operator <<(std::ostream &os, const message &msg) {
	os << "\x1b[" << msg.lvl->colorcode << "m" << std::setw(4) << msg.lvl->name << "\x1b[m ";
	os << msg.filename << ":" << msg.lineno << " ";
	os << "(" << msg.functionname;
	os << ", thread " << msg.thread_id << ")";
	os << ": " << msg.text;

	return os;
}


}} // namespace openage::log
