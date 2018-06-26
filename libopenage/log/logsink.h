// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <list>
#include <mutex>

#include "level.h"

namespace openage {
namespace log {


/**
 * Abstract base for classes that - in one way or an other - print log messages.
 *
 * Instances of this class are automatically added to LogSource::global_sink_list
 * vector by their constructors (and removed by their destructors).
 */
class LogSink {
public:
	LogSink();
	virtual ~LogSink();


	/**
	 * TODO: Add iptables-like chains that decide whether a message will be
	 *       logged, depending on msg.info, logger id, thread id, etc.
	 *       This member variable is only a make-shift solution with
	 *       obvious limitations.
	 */
	void set_loglevel(level loglevel);

private:
	level loglevel;

	/**
	 * Called internally by put_log_message if a message is accepted
	 */
	virtual void output_log_message(const struct message &msg, class LogSource *source) = 0;


	friend class LogSinkList;
};


/**
* Holds a list of all registered log sinks;
* Maintained from the LogSink constructors/destructors.
*/
class LogSinkList {
public:
	static LogSinkList &instance();

	LogSinkList(LogSinkList const&) = delete;

	void operator=(LogSinkList const&) = delete;

	void log(const message &msg, class LogSource *source) const;

	void add(LogSink *sink);

	void remove(LogSink *sink);

	bool supports_loglevel(level loglevel) const;

	void loglevel_changed();

private:
	LogSinkList();

	std::list<LogSink*> sinks;

	mutable std::mutex sinks_mutex;

	void set_lowest_loglevel();

	level lowest_loglevel;
};


}} // namespace openage::log
