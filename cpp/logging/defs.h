// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOGGING_DEFS_H_
#define OPENAGE_LOGGING_DEFS_H_

namespace openage {
namespace logging {

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
 * message metadata
 */
struct MessageInfo {
	size_t msg_id;
	const char *const sourcefile;
	unsigned lineno;
	const char *const functionname;
	unsigned thread_id;
	level lvl;
};


}} // namespace openage::logging

#endif
