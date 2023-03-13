// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "input_context.h"


namespace openage::input {

InputContext::InputContext(const std::string id) :
	id{id},
	by_keyevent{},
	by_mouseevent{},
	by_wheelevent{} {}


const std::string &InputContext::get_id() {
	return this->id;
}

const std::shared_ptr<BindingContext> &InputContext::get_binding_context() {
	return this->binding_context;
}

void InputContext::bind(const KeyEvent &ev, const InputAction act) {
	this->by_keyevent.emplace(std::make_pair(ev, act));
}

void InputContext::bind(const MouseEvent &ev, const InputAction act) {
	this->by_mouseevent.emplace(std::make_pair(ev, act));
}

void InputContext::bind(const WheelEvent &ev, const InputAction act) {
	this->by_wheelevent.emplace(std::make_pair(ev, act));
}

bool InputContext::is_bound(const KeyEvent &ev) const {
	return this->by_keyevent.contains(ev);
}

bool InputContext::is_bound(const MouseEvent &ev) const {
	return this->by_mouseevent.contains(ev);
}

bool InputContext::is_bound(const WheelEvent &ev) const {
	return this->by_wheelevent.contains(ev);
}

const InputAction &InputContext::lookup(const KeyEvent &ev) const {
	return this->by_keyevent.at(ev);
}

const InputAction &InputContext::lookup(const MouseEvent &ev) const {
	return this->by_mouseevent.at(ev);
}

const InputAction &InputContext::lookup(const WheelEvent &ev) const {
	return this->by_wheelevent.at(ev);
}

} // namespace openage::input
