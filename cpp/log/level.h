// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_LEVEL_H_
#define OPENAGE_LOG_LEVEL_H_

#include <iostream>

namespace openage {
namespace log {


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
 * Prints a log level to an output stream.
 */
std::ostream &operator <<(std::ostream &os, level lvl);


}} // namespace openage::log

#endif
