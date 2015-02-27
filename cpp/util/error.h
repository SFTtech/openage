// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_ERROR_H_
#define OPENAGE_UTIL_ERROR_H_

#include <cstdarg>
#include <cstdio>
#include <iostream>

#include "../log/log.h"

namespace openage {
namespace util {


/**
 * Exception type
 */
class Error {
public:
	/**
	 * Constructor.
	 */
	Error(log::MessageBuilder &msg);


	/**
	 * The error message.
	 */
	log::Message msg;


	virtual std::string type_name() const;
};


std::ostream &operator <<(std::ostream &os, const Error &e);


}} // namespace openage::util

#endif
