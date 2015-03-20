// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_LOGSINK_H_
#define OPENAGE_LOG_LOGSINK_H_

#include <vector>
#include <mutex>

#include "logsource.h"

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
	virtual void output_log_message(const Message &msg, LogSource *source) = 0;


	/**
	 * Contains a list of all registered log sinks;
	 * Maintained from the LogSink constructor/destructors.
	 */
	static std::vector<LogSink *> log_sink_list;


	/**
	 * Secures log_sink_list.
	 *
	 * TODO: Use a more efficient multi-read, single-write lock that uses
	 *       atomics to lock the reading.
	 */
	static std::mutex log_sink_list_mutex;


	friend class LogSource;
};


}} // namespace openage::log

#endif
