// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_LEVEL_H_
#define OPENAGE_LOG_LEVEL_H_

#include <iostream>

// pxd: from libopenage.util.enum cimport Enum
#include "../util/enum.h"

namespace openage {
namespace log {


/**
 * Data associated with a log level.
 *
 * pxd:
 *
 * cppclass level_properties:
 *     int priority
 *     const char *name
 *     const char *colorcode
 */
struct level_properties {
	// default constructor for the 'unknown' loglevel.
	level_properties()
		:
		level_properties(0, "unknown loglevel", "5") {}

	level_properties(int priority, const char *name, const char *colorcode)
		:
		priority{priority},
		name{name},
		colorcode{colorcode} {}

	int priority;
	const char *name;
	const char *colorcode;
};

// pxd: ctypedef Enum[level_properties] level
using level = util::Enum<level_properties>;


namespace lvl {

// pxd: level MIN
// pxd: level spam
// pxd: level dbg
// pxd: level info
// pxd: level warn
// pxd: level err
// pxd: level crit
// pxd: level MAX

extern level MIN;  // not designed for use in messages
extern level spam;
extern level dbg;
extern level info;
extern level warn;
extern level err;
extern level crit;
extern level MAX;  // not designed for use in messages

} // lvl


/**
 * Prints a log level to an output stream.
 */
std::ostream &operator <<(std::ostream &os, level lvl);


}} // namespace openage::log

#endif
