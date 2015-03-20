// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "error.h"

#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include "strings.h"
#include "../log/log.h"

namespace openage {
namespace util {


// TODO: exception-hierarchy; automatically storing the cause on throw-again,
//       lightweight exceptions, stacktrace-collecting exceptions.


Error::Error(log::MessageBuilder &msg)
	:
	msg{msg.finalize()} {

	// TODO the logging should actually happen when _catching_ the error.
	log::log(MSG(err) << *this);
}


std::string Error::type_name() const {
	return "Error";
}


std::ostream &operator <<(std::ostream &os, const Error &e) {
	os << e.type_name() << ": " << e.msg;

	return os;
}

} //namespace util
} //namespace openage
