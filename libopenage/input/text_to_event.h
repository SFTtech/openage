// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "event.h"

namespace openage {
namespace input {

/**
 * Convert a string to an event, throw if the string is not a valid event.
 */
Event text_to_event(const std::string &event_str);

}} // openage::input
