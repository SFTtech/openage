// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOGGING_LOGGING_H_
#define OPENAGE_LOGGING_LOGGING_H_

#include <vector>
#include <tuple>
#include <atomic>

#include "../util/constexpr.h"
#include "../util/strings.h"
#include "../util/unique_identifier.h"
#include "../config.h"

namespace openage {
namespace logging {


enum class level {
	MIN,   // not designed for use in messages
	spam,
	dbg,
	info,
	warn,
	err,
	crit,
	MAX    // not designed for use in messages
};


/**
 * Data associated with a log level.
 */
struct level_properties {
	const char *name;
	const char *colorcode;
};


/**
 * Returns data associated with a log level.
 */
level_properties get_level_properties(level lvl);


/**
 * A completed logmessage, containing the actual message, plus all sorts of metadata.
 */
class Message {
public:
	Message(size_t msg_id, const char *sourcefile, unsigned lineno, const char *functionname, level lvl);

	std::string text;

	const size_t msg_id;
	const char *const sourcefile;
	const unsigned lineno;
	const char *const functionname;
	const level lvl;
	const unsigned thread_id;
	const int64_t timestamp;
};


/**
 * Prints a log level to an output stream.
 */
std::ostream &operator <<(std::ostream &os, level lvl);


class ThreadId {
public:
	ThreadId();

	const unsigned val;

private:
	static std::atomic<unsigned> counting_id;
};


extern thread_local const ThreadId current_thread_id;


/**
 * Represents a log message that is still in construction.
 */
class MessageBuilder : public util::OSStreamPtr {
public:
	/**
	 * Upon completion, this object contains the finished message and metadata.
	 */
	Message constructed_message;

public:
	/**
	 * Don't use this constructor directly; use the MSG macro instead.
	 *
	 *
	 * @param msg_id             unique identifier (UNIQUE_VALUE from util/unique_identifier.h).
	 * @param sourcefile, lineno source file name and line number (__FILE__, __LINE__).
	 * @param functionname       (fully qualified) function name (__PRETTY_FUNCTION__).
	 * @param lvl                loglevel of the message. Also required for exception messages.
	 */
	MessageBuilder(size_t msg_id,
	        const char *sourcefile, unsigned lineno, const char *functionname,
	        level lvl = level::info);

	template<typename T>
	MessageBuilder &operator <<(const T &t) {
		this->stream_ptr->stream << t;
		return *this;
	}

	template<typename T>
	MessageBuilder &operator <<(T &(*t)(T &)) {
		this->stream_ptr->stream << t;
		return *this;
	}
};


// macros suck, yes. but there's just no other way to include
// __FILE__, __LINE__ and __PRETTY_FUNCTION__.

#define MSG(LVL) ::openage::logging::MessageBuilder(             \
	UNIQUE_VALUE,                                            \
	::openage::util::constexpr_::strip_prefix(               \
		__FILE__,                                        \
		::openage::config::buildsystem_sourcefile_dir),  \
	__LINE__,                                                \
	__PRETTY_FUNCTION__,                                     \
	::openage::logging::level:: LVL)


class LogSink;


/**
 * Any class that wants to provide .log() shall inherit from this.
 * (example: Engine, Unit, ...).
 */
class Logger {
public:
	Logger();

	// make class polymorphic to provide TypeInfo for dynamic casting.
	virtual ~Logger() = default;

	/**
	 * Logs a message
	 */
	void log(MessageBuilder &msg);

	/**
	 * Initialized during the Logger constructor,
	 * guaranteed to be unique.
	 */
	const size_t logger_id;

	virtual std::string logger_name() = 0;
private:
	/**
	 * Provides unique logger ids.
	 */
	static size_t get_unique_logger_id();

	/**
	 * List of all instantiated LogSink objects.
	 */
	static std::vector<LogSink *> log_sink_list;

	friend class LogSink;
};


/**
 * prints Message to a stream (with color codes and everything)
 */
std::ostream &operator <<(std::ostream &os, const Message &msg);


/**
 * Abstract base for classes that - in one way or an other - print log messages.
 *
 * Instances of this class are automatically added to Logger::global_sink_list
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
	virtual void output_log_message(const Message &msg, Logger *source) = 0;

	friend class Logger;
};

}} // openage::logging

#endif
