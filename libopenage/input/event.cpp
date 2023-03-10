// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "event.h"

#include <functional>
#include <utility>

namespace openage::input {

Event::Event(event_class cl) :
	cl{cl} {}

KeyEvent::KeyEvent(QKeyCombination key) :
	Event(event_class::KEYBOARD),
	key{key} {}

int KeyEvent::hash() const {
	return key.toCombined();
}

bool KeyEvent::operator==(const Event &other) const {
	return this->hash() == other.hash();
}

int event_hash::operator()(const Event &e) const {
	return e.hash();
}

} // namespace openage::input
