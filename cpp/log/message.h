// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_MESSAGE_H_
#define OPENAGE_LOG_MESSAGE_H_

#include <memory>

#include "level.h"
#include "../util/unique_identifier.h"
#include "../config.h"
#include "../util/constexpr.h"
#include "../util/sstreamcache.h"

namespace openage {
namespace log {


/**
 * The meta data for a log/exception message.
 */
struct message_meta_data {
	/**
	 * An ID that is unique to the constructor invocation of the message.
	 */
	const size_t msg_id;

	/**
	 * The filename where the message has been constructed.
	 */
	const char *const sourcefile;

	/**
	 * The line number where the message has been constructed.
	 */
	const unsigned lineno;

	/**
	 * The (pretty) function name where the message has been constructed.
	 */
	const char *const functionname;

	/**
	 * The message's log level.
	 */
	const level lvl;

	/**
	 * A unique id for the thread where the message has been constructed.
	 */
	const unsigned thread_id;

	/**
	 * A (nanosecond-resolution) timestamp of the message construction.
	 */
	const int64_t timestamp;
};


/**
 * A comple log/exception message, containing the actual message, plus message metadata.
 */
class Message {
public:
	// Standard constructor (via MessageBuilder via the MSG macro).
	Message(size_t msg_id, const char *sourcefile, unsigned lineno, const char *functionname, level lvl);

	// Default copy/assignment operators.
	Message(const Message &msg) = default;
	Message &operator =(const Message &msg) = default;

	Message(Message &&msg) = default;
	Message &operator =(Message &&msg) = default;

	/**
	 * The actual message text
	 */
	std::string text;

	message_meta_data meta;
};


/**
 * prints Message to a stream (with color codes and everything!)
 */
std::ostream &operator <<(std::ostream &os, const Message &msg);


/**
 * Represents a log message that is still in construction.
 */
class MessageBuilder {
public:
	/**
	 * Don't use this constructor directly; instead use the MSG macro.
	 *
	 *
	 * @param msg_id             unique identifier (UNIQUE_VALUE from util/unique_identifier.h).
	 * @param sourcefile, lineno source file name and line number (__FILE__, __LINE__).
	 * @param functionname       (fully qualified) function name (__PRETTY_FUNCTION__).
	 * @param lvl                loglevel of the message. Also required for exception messages.
	 */
	MessageBuilder(size_t msg_id,
	               const char *sourcefile, unsigned lineno, const char *functionname,
	               level lvl=level::info);


	// These methods allow usage like an ostream object.
	template<typename T>
	MessageBuilder &operator <<(const T &t) {
		this->init_stream_if_necessary();
		this->str_stream.stream_ptr->stream << t;
		return *this;
	}


	template<typename T>
	MessageBuilder &operator <<(T &(*t)(T &)) {
		this->init_stream_if_necessary();
		this->str_stream.stream_ptr->stream << t;
		return *this;
	}


	// Optimizations to prevent needless stringstream initialization if just a
	// simple string is added.
	MessageBuilder &operator <<(const char *s);
	MessageBuilder &operator <<(const std::string &s);


	// Allow directly inputting 
	template<typename T>
	MessageBuilder &operator <<(const std::unique_ptr<T> &t) {
		*this << t.get();
		return *this;
	}


	template<typename T>
	MessageBuilder &operator <<(const std::shared_ptr<T> &t) {
		*this << t.get();
		return *this;
	}


	/**
	 * Allows printf-style formatting.
	 *
	 * The formatted string is appended to the current result.
	 */
	MessageBuilder &fmt(const char *fmt, ...);


	/**
	 * Appends a string (identical to << s).
	 */
	MessageBuilder &str(const char *s);


	/**
	 * Returns the constructed message.
	 *
	 * Continuing to use the MessageBuilder object after this is possible,
	 * but invalidates the finalized message.
	 *
	 * Designed to be invoked by LogSource.log().
	 */
	Message &finalize();


private:
	/**
	 * Wraps the stringstream object that is used to construct the message
	 * text.
	 */
	util::OSStreamPtr str_stream;


	/**
	 * By default, str_stream is not used.
	 * If only one instance of fmt() or <<(std::string) is used, this saves
	 * significant performance.
	 *
	 * This method is called to initialize the stream and feed it with any
	 * already-existing content.
	 */
	void init_stream_if_necessary();


	/**
	 * Collects the message and metadata.
	 * Obtained through finalize().
	 */
	Message constructed_message;
};


// MSG is resolved to a MessageBuilder constructor invocation;
// it fills in information such as __FILE__, __LINE__ and __PRETTY_FUNCTION_.
//
// Unfortunately, macros are the only way to achieve that.

#define MSG(LVL) \
	::openage::log::MessageBuilder( \
	UNIQUE_VALUE, \
	::openage::util::constexpr_::strip_prefix( \
		__FILE__, \
		::openage::config::buildsystem_sourcefile_dir), \
	__LINE__, \
	__PRETTY_FUNCTION__, \
	::openage::log::level:: LVL)


}} // openage::log

#endif
