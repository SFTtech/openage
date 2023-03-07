// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace openage {
namespace input {

/**
 * highest level classes of input
 */
enum class event_class {
	KEYBOARD,
	MOUSE,
	GUI,
};


struct event_class_hash {
	int operator()(const event_class &s) const;
};


/**
 * Input event, as triggered by some input device like
 * mouse, kezb, joystick, tablet, microwave or dildo.
 * Some modifier keys may also be pressed during the event.
 */
class Event {
public:
	Event(event_class cl);

	/**
	 * logable debug info
	 */
	std::string info() const;

	bool operator==(const Event &other) const;
};


struct event_hash {
	int operator()(const Event &e) const;
};


} // namespace input
} // namespace openage
