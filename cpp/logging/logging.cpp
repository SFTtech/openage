// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "logging.h"

#include <iostream>
#include <iomanip>

#include "../crossplatform/timing.h"

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
	msg_id{msg_id},
	sourcefile{sourcefile},
	lineno{lineno},
	functionname{functionname},
	lvl{lvl},
	thread_id{current_thread_id.val},
	timestamp{timing::get_real_time()} {}


MessageBuilder::MessageBuilder(size_t msg_id,
                 const char *sourcefile,
                 unsigned lineno,
                 const char *functionname,
                 level lvl)
	:
	constructed_message{msg_id, sourcefile, lineno, functionname, lvl} {}


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


std::ostream &operator <<(std::ostream &os, const Message &msg) {
	level_properties props = get_level_properties(msg.lvl);

	os << "\x1b[" << props.colorcode << "m" << std::setw(4) << props.name << "\x1b[m ";
	os << msg.sourcefile << ":" << msg.lineno << " ";
	os << "(" << msg.functionname;
	os << ", thread " << msg.thread_id << ")";
	os << ": " << msg.text;

	return os;
}


Logger::Logger()
	:
	logger_id{Logger::get_unique_logger_id()} {}


void Logger::log(MessageBuilder &msg) {
	msg.constructed_message.text = msg.get();

	// TODO: protect with mutex/atomics?

	// TODO handle situation: log_sink_list empty
	// makeshift solution:
	if (Logger::log_sink_list.size() == 0) {
		std::cout << "no logsink exist. dumping message to stdout." << std::endl;
		std::cout << msg.constructed_message;
	}

	for (LogSink *sink : Logger::log_sink_list) {
		// TODO more sophisticated filtering (iptables-chains-like)
		if (msg.constructed_message.lvl >= sink->loglevel) {
			sink->output_log_message(msg.constructed_message, this);
		}
	}
}


size_t Logger::get_unique_logger_id() {
	// Strictly-monotonically increasing counter.
	static std::atomic<size_t> ctr{0};

	return ctr++;
}


LogSink::LogSink() {
	// TODO mutex this
	Logger::log_sink_list.push_back(this);

	this->loglevel = level::dbg;
}


LogSink::~LogSink() {
	// TODO de-constructing log_sink_list takes O(n^2) time...
	// while this is utterly insignificant, building a map upon
	// start-of-deinitialization might be prettier.

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
