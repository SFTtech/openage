// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "binding_context.h"


namespace openage::input {

BindingContext::BindingContext() :
	by_keyevent{},
	by_mouseevent{},
	by_wheelevent{} {}


void BindingContext::bind(const KeyEvent &ev, const Binding bind) {
	this->by_keyevent.emplace(std::make_pair(ev, bind));
}

void BindingContext::bind(const MouseEvent &ev, const Binding bind) {
	this->by_mouseevent.emplace(std::make_pair(ev, bind));
}

void BindingContext::bind(const WheelEvent &ev, const Binding bind) {
	this->by_wheelevent.emplace(std::make_pair(ev, bind));
}

bool BindingContext::is_bound(const KeyEvent &ev) const {
	return this->by_keyevent.contains(ev);
}

bool BindingContext::is_bound(const MouseEvent &ev) const {
	return this->by_mouseevent.contains(ev);
}

bool BindingContext::is_bound(const WheelEvent &ev) const {
	return this->by_wheelevent.contains(ev);
}

const Binding &BindingContext::lookup(const Event &ev) const {
	if (ev.get_class() == event_class::KEYBOARD) {
		auto input_ev = dynamic_cast<const KeyEvent &>(ev);
		return this->lookup(input_ev);
	}
	else if (ev.get_class() == event_class::MOUSE) {
		auto input_ev = dynamic_cast<const MouseEvent &>(ev);
		return this->lookup(input_ev);
	}
	else if (ev.get_class() == event_class::KEYBOARD) {
		auto input_ev = dynamic_cast<const WheelEvent &>(ev);
		return this->lookup(input_ev);
	}

	throw Error{MSG(err) << "Unrecognized event class."};
}

const Binding &BindingContext::lookup(const KeyEvent &ev) const {
	return this->by_keyevent.at(ev);
}

const Binding &BindingContext::lookup(const MouseEvent &ev) const {
	return this->by_mouseevent.at(ev);
}

const Binding &BindingContext::lookup(const WheelEvent &ev) const {
	return this->by_wheelevent.at(ev);
}

} // namespace openage::input
