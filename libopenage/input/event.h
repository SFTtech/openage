// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <QKeyCombination>
#include <QMouseEvent>
#include <QWheelEvent>

#include "coord/pixel.h"

namespace openage {
namespace input {

/**
 * highest level classes of input
 */
enum class event_class {
	KEYBOARD,
	MOUSE,
	WHEEL,
	GUI,
};


using state_t = int;
using code_t = int;
using modifier_t = int;

/**
 * Input event, as triggered by some input device like
 * mouse, kezb, joystick, tablet, microwave or dildo.
 * Some modifier keys may also be pressed during the event.
 */
class Event {
public:
	Event(const QEvent &ev);

	~Event() = default;

	const std::shared_ptr<QEvent> &get_event() const;

	bool operator==(const Event &other) const;

	event_class cl;
	code_t code; // key/button identifier
	modifier_t mod_code; // modifier keys (CTRL, ALT, SHIFT, ...)
	state_t state; // state of the keypress (pressed, released, double click, wheel up/down, ...)

private:
	std::shared_ptr<QEvent> event;
};


struct event_hash {
	int operator()(const Event &e) const;
};


} // namespace input
} // namespace openage
