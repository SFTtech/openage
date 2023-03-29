// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "input/event.h"

namespace openage::input {

/**
 * Convert a string to an event.
 *
 * TODO: Mouse/Wheel/GUI events
 *
 * @throws if the string is not a valid event.
 */
Event text_to_event(const std::string &event_str);

} // namespace openage::input
