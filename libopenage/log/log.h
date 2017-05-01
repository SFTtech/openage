// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libopenage.log.level cimport level
#include "level.h"
#include "message.h"

namespace openage {
namespace log {

struct message;

/**
 * Convenience method that makes use of the 'general' LogSource.
 *
 * Invokes general_source()->log(msg).
 */
void log(const message &msg);


/**
 * Sets the log level of the global stdout sink.
 *
 * pxd: void set_level(level lvl) except +
 */
void set_level(level lvl);


}} // openage::log
