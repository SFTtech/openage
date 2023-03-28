// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "event.h"

#include <functional>
#include <utility>

namespace openage::input {

int event_class_hash::operator()(const event_class &c) const {
	return std::hash<int>()(static_cast<int>(c));
}


ClassCode::ClassCode(event_class cl, code_t code) :
	cl{cl},
	code{code} {}


std::vector<event_class> ClassCode::get_classes() const {
	std::vector<event_class> result;

	// use event_base to traverse up the class tree
	event_class c = this->cl;
	result.push_back(c);
	while (event_class_rel.count(c) > 0) {
		c = event_class_rel.at(c);
		result.push_back(c);
	}
	return result;
}


bool ClassCode::operator==(const ClassCode &other) const {
	return this->cl == other.cl && this->code == other.code;
}


bool ClassCode::is_subclass(const event_class &other) const {
	for (auto cl : this->get_classes()) {
		if (cl == other) {
			return true;
		}
	}
	return false;
}


int class_code_hash::operator()(const ClassCode &cc) const {
	return std::hash<int>()(static_cast<int>(cc.cl))
	       ^ std::hash<int>()(cc.code) * 3664657;
}


Event::Event(const QEvent &ev) :
	event{std::shared_ptr<QEvent>(ev.clone())} {
	switch (this->event->type()) {
	case QEvent::KeyPress:
	case QEvent::KeyRelease: {
		auto event = dynamic_pointer_cast<QKeyEvent>(this->event);
		this->cc = ClassCode(event_class::KEYBOARD, event->key());
		this->mod_code = event->modifiers();
		this->state = event->type();
	} break;
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease: {
		auto event = dynamic_pointer_cast<QMouseEvent>(this->event);
		this->cc = ClassCode(event_class::MOUSE_BUTTON, event->button());
		this->mod_code = event->modifiers();
		this->state = event->type();
	} break;
	case QEvent::MouseButtonDblClick: {
		auto event = dynamic_pointer_cast<QMouseEvent>(this->event);
		this->cc = ClassCode(event_class::MOUSE_BUTTON_DBL, event->button());
		this->mod_code = event->modifiers();
		this->state = event->type();
	} break;
	case QEvent::MouseMove: {
		auto event = dynamic_pointer_cast<QMouseEvent>(this->event);
		this->cc = ClassCode(event_class::MOUSE_MOVE, event->button());
		this->mod_code = event->modifiers();
		this->state = event->type();
	} break;
	case QEvent::Wheel: {
		auto event = dynamic_pointer_cast<QWheelEvent>(this->event);
		if (event->angleDelta().y() > 0) {
			// forward
			this->cc = ClassCode(event_class::WHEEL, 1);
		}
		else {
			// backwards
			this->cc = ClassCode(event_class::WHEEL, -1);
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
	cc{cl, code},
	mod_code{mod},
	state{state},
	event{nullptr} {}


const std::shared_ptr<QEvent> &Event::get_event() const {
	return this->event;
}


bool Event::operator==(const Event &other) const {
	return this->cc == other.cc
	       && this->mod_code == other.mod_code
	       && this->state == other.state;
}


std::string Event::info() const {
	// TODO: human-readable info

	std::string result = "[Event: ";
	result += "class=" + std::to_string(static_cast<int>(this->cc.cl)) + ", ";
	result += "code=" + std::to_string(this->cc.code) + ", ";
	result += "modset=" + std::to_string(this->mod_code) + ", ";
	result += "state=" + std::to_string(this->state) + "]";
	return result;
}


int event_hash::operator()(const Event &e) const {
	return class_code_hash()(e.cc)
	       ^ std::hash<int>()(e.mod_code)
	       ^ std::hash<int>()(e.state);
}


} // namespace openage::input
