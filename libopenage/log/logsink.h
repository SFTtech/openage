// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
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
	level loglevel;

private:
	/**
	 * Called internally by put_log_message if a message is accepted
	 */
	virtual void output_log_message(const struct message &msg, class LogSource *source) = 0;


	friend class LogSource;
};


/**
* Protects sink_list.
*
* TODO: use a more efficient multi-read, single-write lock
*/
extern std::mutex sink_list_mutex;


/**
* Holds a list of all registered log sinks;
* Maintained from the LogSink constructors/destructors.
*/
std::vector<LogSink *> &sink_list();


}} // namespace openage::log
