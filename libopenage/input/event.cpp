// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "event.h"

#include <functional>
#include <utility>

namespace openage::input {

int event_class_hash::operator()(const event_class &c) const {
	return std::hash<int>()(static_cast<int>(c));
}


Event::Event(const QEvent &ev) :
	event{std::shared_ptr<QEvent>(ev.clone())} {
	switch (this->event->type()) {
	case QEvent::KeyPress:
	case QEvent::KeyRelease: {
		this->cl = event_class::KEYBOARD;
		auto event = dynamic_pointer_cast<QKeyEvent>(this->event);
		this->code = event->key();
		this->mod_code = event->modifiers();
		this->state = event->type();
	} break;
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove: {
		this->cl = event_class::MOUSE;
		auto event = dynamic_pointer_cast<QMouseEvent>(this->event);
		this->code = event->button();
		this->mod_code = event->modifiers();
		this->state = event->type();
	} break;
	case QEvent::Wheel: {
		this->cl = event_class::WHEEL;
		auto event = dynamic_pointer_cast<QWheelEvent>(this->event);
		if (event->angleDelta().y() > 0) {
			// forward
			this->code = 1;
		}
		else {
			// backwards
			this->code = -1;
		}
		this->mod_code = event->modifiers();
		this->state = event->type();
	} break;
	// TODO: GUI events
	default:
		throw Error{MSG(err) << "Unrecognized input event type."};
	}
}

Event::Event(event_class cl, code_t code, modset_t mod, state_t state) :
	cl{cl},
	code{code},
	mod_code{mod},
	state{state},
	event{nullptr} {}


const std::shared_ptr<QEvent> &Event::get_event() const {
	return this->event;
}


bool Event::operator==(const Event &other) const {
	return this->cl == other.cl
	       && this->code == other.code
	       && this->mod_code == other.mod_code
	       && this->state == other.state;
}


std::string Event::info() const {
	// TODO: human-readable info

	std::string result = "[Event: ";
	result += "class=" + std::to_string(static_cast<int>(this->cl)) + ", ";
	result += "code=" + std::to_string(this->code) + ", ";
	result += "modset=" + std::to_string(this->mod_code) + ", ";
	result += "state=" + std::to_string(this->state) + "]";
	return result;
}


int event_hash::operator()(const Event &e) const {
	return std::hash<int>()(static_cast<int>(e.cl))
	       ^ std::hash<int>()(e.code) * 3664657
	       ^ std::hash<int>()(e.mod_code)
	       ^ std::hash<int>()(e.state);
}


} // namespace openage::input
