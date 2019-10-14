// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "event.h"

#include <functional>
#include <utility>

namespace openage::input {

ClassCode::ClassCode(event_class cl, code_t code)
	:
	eclass(cl),
	code(code) {
}


std::vector<event_class> ClassCode::get_classes() const {
	std::vector<event_class> result;

	// use event_base to traverse up the class tree
	event_class ec = this->eclass;
	result.push_back(ec);
	while (event_base.count(ec) > 0) {
		ec = event_base.at(ec);
		result.push_back(ec);
	}
	return result;
}


bool ClassCode::has_class(const event_class &ec) const {
	for (auto c : this->get_classes()) {
		if (c == ec) {
			return true;
		}
	}
	return false;
}


bool operator ==(ClassCode a, ClassCode b) {
	return a.eclass == b.eclass && a.code == b.code;
}


Event::Event(event_class cl, code_t code, modset_t mod)
	:
	cc(cl, code),
	mod(std::move(mod)) {}


Event::Event(event_class cl, std::string text, modset_t mod)
	:
	cc(cl, 0),
	mod(std::move(mod)),
	utf8(std::move(text)) {}


char Event::as_char() const {
	return (cc.code & 0xff);
}


std::string Event::as_utf8() const {
	return utf8;
}


std::string Event::info() const {
	std::string result;
	result += "[Event: ";
	result += std::to_string(static_cast<int>(this->cc.eclass));
	result += ", ";
	result += std::to_string(this->cc.code);
	result += ", ";
	result += this->as_char();
	result += " (";
	result += std::to_string(this->mod.size());
	result += ")]";
	return result;
}


bool Event::operator ==(const Event &other) const {
	return this->cc == other.cc && this->mod == other.mod && this->utf8 == other.utf8;
}


int event_hash::operator()(const Event &e) const {
	return class_code_hash()(e.cc); // ^ std::hash<modset_t>()(e.mod) * 3664657;
}


int event_class_hash::operator()(const event_class& c) const {
	return std::hash<int>()(static_cast<int>(c));
}


int modifier_hash::operator()(const modifier &m) const {
	return std::hash<int>()(static_cast<int>(m));
}


int class_code_hash::operator()(const ClassCode &e) const {
	return event_class_hash()(e.eclass) ^
	       std::hash<int>()(e.code) * 3664657;
}


modset_t sdl_mod(SDL_Keymod mod) {
	// Remove modifiers like num lock and caps lock
	// mod = static_cast<SDL_Keymod>(mod & this->used_keymods);
	modset_t result;
	if (mod & KMOD_CTRL) {
		result.emplace(modifier::CTRL);
	}
	if (mod & KMOD_SHIFT) {
		result.emplace(modifier::SHIFT);
	}
	if (mod & KMOD_ALT) {
		result.emplace(modifier::ALT);
	}
	return result;
}


Event sdl_key(SDL_Keycode code, SDL_Keymod mod) {

	// sdl values for non printable keys
	if (code & (1 << 30)) {
		return Event(event_class::OTHER, code, sdl_mod(mod));
	}
	else {
		event_class ec;
		char c = (code & 0xff);
		if (isdigit(c)) {
			ec = event_class::DIGIT;
		}
		else if (isalpha(c)) {
			ec = event_class::ALPHA;
		}
		else if (isprint(c)) {
			ec = event_class::PRINT;
		}
		else {
			ec = event_class::NONPRINT;
		}
		return Event(ec, code, sdl_mod(mod));
	}
}

Event utf8(const std::string &text) {
	return Event(event_class::UTF8, text, modset_t());
}

Event sdl_mouse(int button, SDL_Keymod mod) {
	return Event(event_class::MOUSE_BUTTON, button, sdl_mod(mod));
}

Event sdl_mouse_up_down(int button, bool up, SDL_Keymod mod) {
	return Event(up ? event_class::MOUSE_BUTTON_UP : event_class::MOUSE_BUTTON_DOWN, button, sdl_mod(mod));
}

Event sdl_wheel(int direction, SDL_Keymod mod) {
	return Event(event_class::MOUSE_WHEEL, direction, sdl_mod(mod));
}


} // openage::input
