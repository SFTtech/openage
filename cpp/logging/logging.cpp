// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "logging.h"

#include <iomanip>
#include <iostream>


namespace openage {
namespace logging {


std::ostream &operator <<(std::ostream &os, level lvl) {
	level_properties lvl_properties = get_level_properties(lvl);
	os << lvl_properties.name;
	return os;
}


ThreadId::ThreadId()
	:
	val{ThreadId::counting_id++} {}


std::atomic<unsigned> ThreadId::counting_id{0};
const thread_local ThreadId current_thread_id;


Message::Message(size_t msg_id,
                 const char *sourcefile,
                 unsigned lineno,
                 const char *functionname,
                 level lvl)
	:
	info{msg_id, sourcefile, lineno, functionname, current_thread_id.val, lvl} {}


level_properties get_level_properties(level lvl) {
	switch (lvl) {
	case level::MIN:  return {">min",     "5"};
	case level::spam: return {"SPAM",     ""};
	case level::dbg:  return {"DBG",      ""};
	case level::info: return {"INFO",     ""};
	case level::warn: return {"WARN",     "33"};
	case level::err:  return {"ERR",      "31;1"};
	case level::crit: return {"CRIT",     "31;1;47"};
	case level::MAX:  return {">max",     "5"};
	default:          return {">unknown", "5"};
	}
}


LoggedMessage::LoggedMessage(const Message &msg)
	:
	info(msg.info),
	text{msg.get()} {}


std::ostream &operator <<(std::ostream &os, const LoggedMessage &msg) {
	level_properties props = get_level_properties(msg.info.lvl);

	os << "\x1b[" << props.colorcode << "m" << std::setw(4) << props.name << "\x1b[m ";
	os << msg.info.sourcefile << ":" << msg.info.lineno << " ";
	os << "(" << msg.info.functionname;
	os << ", thread " << msg.info.thread_id << ")";
	os << ": " << msg.text.c_str();

	return os;
}


Logger::Logger()
	:
	logger_id{Logger::get_unique_logger_id()} {}


void Logger::log(const Message &msg) {
	for (LogSink *sink : Logger::log_sink_list) {
		if (msg.info.lvl >= sink->loglevel) {
			sink->output_log_message(msg, this);
		}
	}
}


size_t Logger::get_unique_logger_id() {
	// Strictly-monotonically increasing counter.
	static std::atomic<size_t> ctr{0};

	return ctr++;
}


LogSink::LogSink() {
	Logger::log_sink_list.push_back(this);

	this->loglevel = level::dbg;
}


LogSink::~LogSink() {
	for (size_t i = 0; i < Logger::log_sink_list.size(); i++) {
		if (Logger::log_sink_list[i] == this) {
			// replace the element with the last element on
			// the vector.
			Logger::log_sink_list[i] = Logger::log_sink_list[Logger::log_sink_list.size() - 1];
			// delete the last element on the vector.
			Logger::log_sink_list.pop_back();

			return;
		}
	}

	// we didn't find the source in the global sink list.

	// it should have been there!

	// seriously, I'm freaked out!

	// TODO log the logger error
}


std::vector<LogSink *> Logger::log_sink_list;


} // namespace logging
} // namespace openage
