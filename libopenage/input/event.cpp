// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "event.h"

#include <functional>
#include <utility>

namespace openage::input {

Event::Event(event_class cl) :
	cl{cl} {}

Event::Event(event_class cl,
             coord::input mouse_position,
             coord::input_delta mouse_motion) :
	cl{cl},
	mouse_position{mouse_position},
	mouse_motion{mouse_motion} {}

event_class Event::get_class() const {
	return this->cl;
}


KeyEvent::KeyEvent(const QKeyCombination key) :
	Event(event_class::KEYBOARD),
	key{key} {}

int KeyEvent::hash() const {
	return key.toCombined();
}

bool KeyEvent::operator==(const Event &other) const {
	return this->hash() == other.hash();
}


MouseEvent::MouseEvent(const QMouseEvent &ev) :
	Event(event_class::MOUSE),
	buttons{ev.buttons()},
	mods{ev.modifiers()} {
}

int MouseEvent::hash() const {
	return this->buttons ^ this->mods;
}

bool MouseEvent::operator==(const Event &other) const {
	return this->hash() == other.hash();
}


WheelEvent::WheelEvent(const QWheelEvent &ev) :
	Event(event_class::WHEEL),
	buttons{ev.buttons()},
	mods{ev.modifiers()} {}

int WheelEvent::hash() const {
	if (this->angle_delta < 0) {
		return -1 * this->buttons ^ this->mods;
	}
	return this->buttons ^ this->mods;
}

bool WheelEvent::operator==(const Event &other) const {
	return this->hash() == other.hash();
}

int event_hash::operator()(const Event &e) const {
	return e.hash();
}


} // namespace openage::input
