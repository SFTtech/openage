// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOGGING_LOGGING_H_
#define OPENAGE_LOGGING_LOGGING_H_

#include <vector>
#include <tuple>
#include <atomic>

#include "../util/constexpr.h"
#include "../util/strings.h"
#include "../util/unique_identifier.h"
#include "defs.h"

namespace openage {
namespace logging {


/**
 * loglevel metadata
 */
struct level_properties {
	const char *name;
	const char *colorcode;
};


/**
 * returns metadata for a log level
 */
level_properties get_level_properties(level lvl);


/**
 * prints log level to stream
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


class Message : public util::OSStreamPtr {
public:
	MessageInfo info;

public:
	/**
	 * constructor.
	 * don't use this directly; use the MSG macro instead.
	 *
	 *
	 * @param msg_id             unique identifier (UNIQUE_VALUE from util/unique_identifier.h).
	 * @param sourcefile, lineno source file name and line number (__FILE__, __LINE__).
	 * @param functionname       (fully qualified) function name (__PRETTY_FUNCTION__).
	 * @param lvl                loglevel of the message. Also required for exception messages.
	 */
	Message(size_t msg_id,
	        const char *sourcefile, unsigned lineno, const char *functionname,
	        level lvl = level::info);

	template<typename T>
	Message &operator <<(const T &t) {
		this->stream_ptr->stream << t;
		return *this;
	}

	template<typename T>
	Message &operator <<(T &(*t)(T &)) {
		this->stream_ptr->stream << t;
		return *this;
	}
};

// we don't need/want any linkage for these compile-time constants
namespace {

// important: if you rename this file or move it to an other directory,
// you MUST adapt the constant literal below.
constexpr const char *logging_h_relative_filename = "cpp/logging/logging.h";

constexpr openage::util::constexpr_::truncated_string_literal cpp_filename_prefix = openage::util::constexpr_::get_prefix(__FILE__, logging_h_relative_filename);

} // anonymous namespace


// macros suck, yes. but there's just no other way to include
// __FILE__, __LINE__ and __PRETTY_FUNCTION__.

#define MSG(LVL) ::openage::logging::Message(                    \
	UNIQUE_VALUE,                                            \
	::openage::util::constexpr_::strip_prefix(               \
		__FILE__,                                        \
		::openage::logging::cpp_filename_prefix),        \
	__LINE__,                                                \
	__PRETTY_FUNCTION__,                                     \
	::openage::logging::level:: LVL)


class LoggedMessage {
public:
	const MessageInfo info;
	const std::string text;

	LoggedMessage(const Message &msg);
};


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
	void log(const Message &msg);

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
 * prints LoggedMessage to a stream (with color codes and everything)
 */
std::ostream &operator <<(std::ostream &os, const LoggedMessage &msg);

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
	virtual void output_log_message(const LoggedMessage &msg, Logger *source) = 0;

	friend class Logger;
};

}} // openage::logging

#endif
