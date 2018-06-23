// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <atomic>
#include <memory>
#include <type_traits>

#include "../util/compiler.h"

#include "externalsstream.h"
#include "strings.h"

namespace openage {
namespace util {


/**
 * Designed for usage by StringFormatter.
 *
 * The problem with iostreams, including ExternalOStringStream, is that
 * they use lots of memory, and are quite slow to construct, due to some
 * pesky implementation details.
 *
 * This class fixes this by keeping a reservoir of such objects,
 * and providing access to them via acquire(), in a thread-safe manner.
 * If the cache is exhausted, new objects are constructed on the heap as needed.
 *
 * release() makes a cached object available for acquiring again (cache) or
 * destroys it (heap).
 */
class CachableOSStream {
public:
	ExternalOStringStream stream;

	/*
	 * Does not call stream.use_with(); for use by cache objects.
	 */
	CachableOSStream();

	/**
	 * Calls stream.use_with(output); for use by heap objects.
	 */
	CachableOSStream(std::string &output);

	/**
	 * Returns a brand-new(*) CachableOSStream object.
	 * Origin may vary (static internal cache, heap allocation).
	 *
	 * After you're done, pass the pointer to release().
	 *
	 * (*) Disclaimer: Object might not actually be brand-new.
	 *
	 * @param output:
	 *     The string object that the stream should output its data to.
	 */
	static CachableOSStream *acquire(std::string &output);

	/**
	 * Resets the stream to a brand-new state, and clears the flag.
	 *
	 * no-op if cs is nullptr.
	 */
	static OAAPI void release(CachableOSStream *cs);

private:
	/**
	 * true if all of the following are true:
	 *  - object is part of a larger cache
	 *  - object is currently in use (-> unavailable)
	 * http://en.cppreference.com/w/cpp/atomic/ATOMIC_FLAG_INIT
	 * https://stackoverflow.com/questions/24437396/stdatomic-flag-as-member-variable
	 */
	std::atomic_flag flag = ATOMIC_FLAG_INIT;
};


/**
 * Wraps an output string stream, and provides all sorts of overloads
 * for operator <<, plus some other formatting methods.
 *
 * Designed mainly for usage by the Formatter / logging / MSG system.
 *
 * The operator << methods return references to ChildType.
 *
 * If possible, input data is written directly to the buffer,
 * but if needed, a CachableOSStream is acquired (and later released).
 * As an optimization, instead of creating a new ExternalOStringStream object,
 * CachableOSStream.acquire() is used internally.
 */
template<typename ChildType>
class StringFormatter {
public:
	/**
	 * @param buffer
	 *     All input data is appended to this object.
	 */
	StringFormatter(std::string &output)
		:
		output{&output},
		stream_ptr{nullptr} {}

	/**
	 * Releases the CachableOSStream object (if it was acquired).
	 */
	virtual ~StringFormatter() {
		CachableOSStream::release(this->stream_ptr);
	}

	inline ChildType &child_type_ref() {
		return *static_cast<ChildType *>(this);
	}

	StringFormatter(StringFormatter<ChildType> &&other) noexcept
		:
		output{other.output},
		stream_ptr{other.stream_ptr} {

		other.stream_ptr = nullptr;
	}

	StringFormatter<ChildType> &operator =(StringFormatter<ChildType> &&other) noexcept {
		this->output = other.output;

		this->stream_ptr = other.stream_ptr;
		other.stream_ptr = nullptr;
		return *this;
	}

	// no copy construction!
	StringFormatter(const StringFormatter &) = delete;
	StringFormatter &operator =(const StringFormatter &) = delete;

	// These methods allow usage like an ostream object.
	template<typename T>
	ChildType &operator <<(const T &t) {
		if (this->should_format()) {
			this->ensure_stream_obj();
			this->stream_ptr->stream << t;
		}
		return this->child_type_ref();
	}


	ChildType &operator <<(std::ios &(*x)(std::ios &)) {
		if (this->should_format()) {
			this->ensure_stream_obj();
			this->stream_ptr->stream << x;
		}
		return this->child_type_ref();
	}


	ChildType &operator <<(std::ostream &(*x)(std::ostream &)) {
		if (this->should_format()) {
			this->ensure_stream_obj();
			this->stream_ptr->stream << x;
		}
		return this->child_type_ref();
	}


	// Optimizations to prevent needless stream-acquiring if just a simple
	// string is printed.
	ChildType &operator <<(const char *s) {
		if (this->should_format()) {
			this->output->append(s);
		}
		return this->child_type_ref();
	}


	ChildType &operator <<(const std::string &s) {
		if (this->should_format()) {
			this->output->append(s);
		}
		return this->child_type_ref();
	}


	// Printf-style formatting
	ChildType &fmt(const char *fmt, ...) ATTRIBUTE_FORMAT(2, 3) {
		if (this->should_format()) {
			va_list ap;
			va_start(ap, fmt);
			util::vsformat(fmt, ap, *this->output);
			va_end(ap);
		}

		return this->child_type_ref();
	}


	// Allow direct inputting of stuff that's wrapped in the C++11 pointer types.
	template<typename T>
	ChildType &operator <<(const std::unique_ptr<T> &ptr) {
		if (this->should_format()) {
			*this << ptr.get();
		}
		return this->child_type_ref();
	}


	template<typename T>
	ChildType &operator <<(const std::shared_ptr<T> &ptr) {
		if (this->should_format()) {
			*this << ptr.get();
		}
		return this->child_type_ref();
	}


	/**
	 * Clears the underlying stream object's flags, if such a stream
	 * object exists.
	 */
	void reset_flags() {
		if (this->stream_ptr != nullptr) {
			this->stream_ptr->stream.clear();
		}
	}

	/**
	* Returns if formatting should actually occur.
	*/
	virtual bool should_format() const {
		return true;
	}

private:
	/**
	 * Ensures that we have a valid CachableOSStream object in stream_ptr.
	 */
	inline void ensure_stream_obj() {
		if (unlikely(this->stream_ptr == nullptr)) {
			this->stream_ptr = CachableOSStream::acquire(*this->output);
		}
	}

	std::string *output;
	CachableOSStream *stream_ptr;
};


/**
 * Allows simple direct usage of StringFormatter.
 */
class Formatter : public StringFormatter<Formatter> {};


/**
 * A self-formatting string! Get yours today!
 *
 * Use like this:
 *
 * std::string stuff() {
 *     FString s;
 *     s << "test: " << 5 << std::cout;
 *     return s;
 * }
 *
 * Use as a faster, care-free replacement for stringstream:
 *
 * std::string stuff() {
 *     std::stringstream sstr;
 *     sstr << "test: " << 5 << std::cout;
 *     return sstr.str();
 * }
 */
class FString : public StringFormatter<FString> {
public:
	FString()
		:
		StringFormatter<FString>{this->buffer} {}

	// allow assignment and construction from std::string.

	FString(const std::string &other)
		:
		StringFormatter<FString>{this->buffer},
		buffer{other} {}

	FString(std::string &&other) noexcept
		:
		StringFormatter<FString>{this->buffer},
		buffer{std::move(other)} {}

	FString &operator =(const std::string &other) {
		this->buffer = other;
		return *this;
	}

	FString &operator =(std::string &&other) noexcept {
		this->buffer = std::move(other);
		return *this;
	}

	// allow conversion to std::string.

	operator std::string &() {
		return this->buffer;
	}

	operator const std::string &() const {
		return this->buffer;
	}

	operator std::string () && {
		return std::move(this->buffer);
	}

	void reset() {
		buffer.clear();
		this->reset_flags();
	}

	// users may directly access this.
	std::string buffer;
};


}} // namespace openage::util
