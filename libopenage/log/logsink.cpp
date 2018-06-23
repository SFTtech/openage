// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "logsink.h"
#include "message.h"

namespace openage {
namespace log {


LogSink::LogSink() {
	LogSinkList::instance().add(this);

	this->set_loglevel(level::dbg);
}


LogSink::~LogSink() {
	LogSinkList::instance().remove(this);
}


void LogSink::set_loglevel(level loglevel) {
	this->loglevel = loglevel;
	LogSinkList::instance().loglevel_changed();
}


LogSinkList::LogSinkList() {
	this->set_lowest_loglevel();
}


LogSinkList &LogSinkList::instance() {
	static LogSinkList instance;
	return instance;
}


void LogSinkList::log(const message &msg, class LogSource *source) const {
	std::lock_guard<std::mutex> lock(this->sinks_mutex);
	for (auto *sink : this->sinks) {
		// TODO: more sophisticated filtering (iptables-chains-like)
		if (msg.lvl >= sink->loglevel) {
			sink->output_log_message(msg, source);
		}
	}
}


void LogSinkList::add(LogSink *sink) {
	std::lock_guard<std::mutex> lock(this->sinks_mutex);
	this->sinks.push_back(sink);
	this->set_lowest_loglevel();
}


void LogSinkList::loglevel_changed() {
	std::lock_guard<std::mutex> lock(this->sinks_mutex);
	this->set_lowest_loglevel();
}


void LogSinkList::set_lowest_loglevel() {
	this->lowest_loglevel = level::MAX;
	for (auto *sink : this->sinks) {
		this->lowest_loglevel = std::min(this->lowest_loglevel, sink->loglevel);
	}
}


void LogSinkList::remove(LogSink *sink) {
	std::lock_guard<std::mutex> lock(this->sinks_mutex);
	this->sinks.remove(sink);
	this->set_lowest_loglevel();
}


bool LogSinkList::supports_loglevel(level loglevel) const {
	std::lock_guard<std::mutex> lock(this->sinks_mutex);
	return loglevel >= this->lowest_loglevel;
}

}} // namespace openage::log
