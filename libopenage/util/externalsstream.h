// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <iostream>
#include <string>

#ifndef OPENAGE_UTIL_EXTERNALSSTREAM_H_
#define OPENAGE_UTIL_EXTERNALSSTREAM_H_

namespace openage {
namespace util {


/**
 * Used by ExternalOStringStream; data is written to the external accumulator.
 *
 * If this object's write functionality is accessed before output has been
 * manually set to a valid std::string object, or if the lifetime of that
 * object has been exceeded, undefined behavior will result.
 */
class ExternalStringBuf : public std::streambuf {
public:
	std::string *output;

	int overflow(int chr) override {
		output->push_back(static_cast<char>(chr));
		return chr;
	}

	std::streamsize xsputn(const char *s, std::streamsize count) override {
		output->append(s, count);
		return count;
	}
};


/**
 * Similar to std::ostringstream, but data is written to an external string
 * object.
 *
 * Using this stream before calling use_with will result in undefined behavior.
 */
class ExternalOStringStream : public std::ostream {
public:
	/**
	 * Creates a stream without a valid accumulator.
	 */
	explicit ExternalOStringStream()
		:
		std::ostream{&this->buf} {}

	/**
	 * Resets the stream's flags, and sets ptr as the internally-used
	 * accumulator.
	 */
	void use_with(std::string &output) {
		this->buf.output = &output;
	}

private:
	ExternalStringBuf buf;
};


}} // openage::util

#endif
