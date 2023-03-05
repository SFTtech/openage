// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "input/legacy/event.h"

namespace openage {
namespace input::legacy {

/**
 * Convert a string to an event, throw if the string is not a valid event.
 */
Event text_to_event(const std::string &event_str);

} // namespace input::legacy
} // namespace openage
