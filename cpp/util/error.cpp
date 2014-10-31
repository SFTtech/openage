// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#include "error.h"

#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include "strings.h"
#include "../log.h"

namespace openage {
namespace util {

Error::Error(const char *fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	this->buf = vformat(fmt, vl);
	va_end(vl);
	log::msg("ERROR: %s", buf);
}

Error::Error(const Error &other) {
	buf = copy(other.buf);
}

Error &Error::operator=(const Error &other) {
	if (this != &other) {
		delete[] buf;
		buf = copy(other.buf);
	}
	return *this;
}

Error::Error(Error &&other) {
	buf = other.buf;
	other.buf = NULL;
}


Error::~Error() {
	delete[] buf;
}

const char *Error::str() const {
	return buf;
}

} //namespace util
} //namespace openage
