// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

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

extern OAAPI level MIN;  // not designed for use in messages
extern OAAPI level spam;
extern OAAPI level dbg;
extern OAAPI level info;
extern OAAPI level warn;
extern OAAPI level err;
extern OAAPI level crit;
extern OAAPI level MAX;  // not designed for use in messages

} // lvl


/**
 * Prints a log level to an output stream.
 */
std::ostream &operator <<(std::ostream &os, level lvl);


}} // namespace openage::log
