// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libc.stdint cimport int64_t
#include <cstdint>
// pxd: from libcpp.string cimport string
#include <string>

#include "config.h"
#include "../util/constexpr.h"
#include "../util/stringformatter.h"
#include "logsink.h"

// pxd: from libopenage.log.level cimport level
#include "level.h"

#if defined(__GNUC__)
#define OPENAGE_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define OPENAGE_FUNC_NAME __FUNCSIG__
#else
#define OPENAGE_FUNC_NAME __FUNCTION__
#endif

namespace openage {


// forward-declaration for use in 'friend' declaration below.
namespace error { class Error; }


namespace log {


/**
 * A complete log/exception message, containing a text message and metadata.
 *
 * The preferred way of creating a `message` object is to use MessageBuilder
 * via the MSG macro.
 * MessageBuilder is auto-converted to message when needed.
 *
 * pxd:
 *
 * cppclass message:
 *    string text
 *    char *filename
 *    unsigned lineno
 *    const char *functionname
 *    level lvl
 *    size_t thread_id
 *    int64_t timestamp
 *
 *    void init() except +
 *    void init_with_metadata_copy(string filename, string functionname) except +
 */
struct OAAPI message {
	std::string text;

	/**
	 * The filename where the message has been constructed.
	 */
	const char *filename;

	/**
	 * The line number where the message has been constructed.
	 */
	unsigned lineno;

	/**
	 * The (pretty) function name where the message has been constructed.
	 */
	const char *functionname;

	/**
	 * The log level.
	 */
	level lvl;

	/**
	 * A unique id for the thread where the message has been constructed.
	 */
	size_t thread_id;

	/**
	 * A (nanosecond-resolution) timestamp of the message construction.
	 */
	int64_t timestamp;

	/**
	 * Sets all members except for filename, lineno, functionname and level.
	 */
	void init();

	/**
	 * Sets all members except for lineno and level.
	 *
	 * filename and functionname are copied to an internal cache.
	 *
	 * Designed to be used when filename and functionname are temporary
	 * objects (e.g. from Python).
	 */
	void init_with_metadata_copy(const std::string &filename, const std::string &functionname);
};


/**
 * prints message to a stream (with color codes and everything!)
 */
std::ostream &operator <<(std::ostream &os, const message &msg);

/**
 * Wrapper around a log message that allows appending to the message with operator <<.
 *
 * Auto-converts to message; via the MSG macro, this is the preferred way of creating
 * log and exc messages.
 */
class OAAPI MessageBuilder : public util::StringFormatter<MessageBuilder> {
public:
	/**
	 * Don't use this constructor directly; instead use the MSG macro.
	 *
	 *
	 * @param filename, lineno source file name and line number (__FILE__, __LINE__).
	 * @param functionname       (fully qualified) function name (OPENAGE_FUNC_NAME).
	 * @param lvl                loglevel of the message. Also required for exception messages.
	 */
	MessageBuilder(const char *filename, unsigned lineno, const char *functionname,
	               level lvl=level::info);

	// auto-convert to message
	inline operator const message &() const {
		return this->msg;
	}

	inline operator message &() {
		return this->msg;
	}

	inline bool should_format() const override {
		// only format if this message will actually be logged
		return LogSinkList::instance().supports_loglevel(this->msg.lvl);
	}

private:
	message msg;

	friend error::Error;
	friend class LogSource;
};


// MSG is resolved to a MessageBuilder constructor invocation;
// it fills in information such as __FILE__, __LINE__ and OPENAGE_FUNC_NAME.
//
// Unfortunately, macros are the only way to achieve that.


// for use with existing log::level objects
#define MSG_LVLOBJ(LVLOBJ) \
	::openage::log::MessageBuilder( \
	::openage::util::constexpr_::strip_prefix( \
		__FILE__, \
		::openage::config::buildsystem_sourcefile_dir), \
	__LINE__, \
	OPENAGE_FUNC_NAME, \
	LVLOBJ)


// for use with log::level literals (auto-prefixes full qualification)
#define MSG(LVL) MSG_LVLOBJ(::openage::log::level:: LVL)


// some convenience shorteners for MSG(...).
#define SPAM MSG(spam)
#define DBG MSG(dbg)
#define INFO MSG(info)
#define WARN MSG(warn)
#define ERR MSG(err)
#define CRIT MSG(crit)


}} // openage::log
