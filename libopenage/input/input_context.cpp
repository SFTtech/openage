// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "input_context.h"

#include "input/legacy/input_manager.h"


namespace openage::input {

InputContext::InputContext() :
	by_keyevent{} {}


void InputContext::bind(const KeyEvent &ev, const InputAction act) {
	this->by_keyevent.emplace(std::make_pair(ev, act));
}

bool InputContext::is_bound(const KeyEvent &ev) const {
	return this->by_keyevent.contains(ev);
}

const InputAction &InputContext::lookup(const KeyEvent &ev) const {
	return this->by_keyevent.at(ev);
}

} // namespace openage::input
