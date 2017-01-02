// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <algorithm>
#include <array>

#include "../log/log.h"
#include "action.h"
#include "input_manager.h"
#include "text_to_event.h"


namespace openage {
namespace input {

InputManager::InputManager(ActionManager *action_manager)
	:
	action_manager{action_manager},
	global_context{this},
	relative_mode{false} {

	this->global_context.register_to(this);
}


namespace {

std::string mod_set_string(modset_t mod) {
	if (not mod.empty()) {
		for (auto &it : mod) {
			switch (it) {
			case modifier::ALT:
				return "ALT + ";
				break;
			case modifier::CTRL:
				return "CTRL + ";
				break;
			case modifier::SHIFT:
				return "SHIFT + ";
				break;
			}
		}
	}

	return "";
}

std::string event_as_string(const Event& event) {
	if (not event.as_utf8().empty()) {
		return mod_set_string(event.mod) + event.as_utf8();
	}
	else {
		if (event.cc.eclass == event_class::MOUSE_WHEEL) {
			if (event.cc.code == -1) {
				return mod_set_string(event.mod) + "Wheel down";
			} else {
				return mod_set_string(event.mod) + "Wheel up";
			}
		}
		return mod_set_string(event.mod) + SDL_GetKeyName(event.cc.code);
	}
}

} // anonymous ns


std::string InputManager::get_bind(const std::string &action_str) {
	action_t action = this->action_manager->get(action_str);
	if (this->action_manager->is("UNDEFINED", action)) {
		return "";
	}

	auto it = this->keys.find(action);
	if (it == this->keys.end()) {
		return " ";
	}

	switch (it->second.cc.eclass) {
		case event_class::MOUSE_BUTTON :
			return this->mouse_bind_to_string(it->second);
		case event_class::MOUSE_WHEEL :
			return this->wheel_bind_to_string(it->second);
		default:
			return this->key_bind_to_string(it->second);
	}
	return "";
}

bool InputManager::set_bind(const std::string &bind_str, const std::string action_str) {
	try {
		action_t action = this->action_manager->get(action_str);
		if (this->action_manager->is("UNDEFINED", action)) {
			return false;
		}

		Event ev = text_to_event(bind_str);

		auto it = this->keys.find(action);
		if (it != this->keys.end()) {
			this->keys.erase(it);
		}
		this->keys.emplace(std::make_pair(action, ev));

		return true;
	}
	catch (int error) {
		return false;
	}
}

std::string InputManager::key_bind_to_string(const Event &ev) {
	std::string key_str = std::string(SDL_GetKeyName(ev.cc.code));

	auto end = ev.mod.end();
	if (ev.mod.find(modifier::ALT) != end) {
		key_str = "Alt "+key_str;
	}
	if (ev.mod.find(modifier::SHIFT) != end) {
		key_str = "Shift "+key_str;
	}
	if (ev.mod.find(modifier::CTRL) != end) {
		key_str = "Ctrl "+key_str;
	}
	return key_str;
}

std::string InputManager::mouse_bind_to_string(const Event &ev) {
	return "MOUSE " + std::to_string(ev.cc.code);
}

std::string InputManager::wheel_bind_to_string(const Event &ev) {
	std::string base = "WHEEL ";
	switch (ev.cc.code) {
	case 1:
		return base + "UP";
	case -1:
		return base + "DOWN";
	default:
		return "";
	}
}

InputContext &InputManager::get_global_context() {
	return this->global_context;
}

InputContext &InputManager::get_top_context() {
	// return the global input context
	// if no override is pushed
	if (this->contexts.empty()) {
		return this->global_context;
	}
	return *this->contexts.back();
}

void InputManager::push_context(InputContext *context) {
	// push the context to the top
	this->contexts.push_back(context);

	context->register_to(this);
}


void InputManager::remove_context(InputContext *context) {
	if (this->contexts.empty()) {
		return;
	}

	for (auto it = this->contexts.begin(); it != this->contexts.end(); ++it) {
		if ((*it) == context) {
			this->contexts.erase(it);
			context->unregister();
			return;
		}
	}
}


bool InputManager::ignored(const Event &e) {
	// filter duplicate utf8 events
	// these are ignored unless the top mode enables
	// utf8 mode, in which case regular char codes are ignored
	return ((e.cc.has_class(event_class::CHAR) ||
	         e.cc.has_class(event_class::UTF8)) &&
	        this->get_top_context().utf8_mode != e.cc.has_class(event_class::UTF8));
}


bool InputManager::trigger(const Event &e) {
	if (this->ignored(e)) {
		return false;
	}

	// arg passed to receivers
	action_arg_t arg{e, this->mouse_position, this->mouse_motion, {}};

	for (auto &it : this->keys) {
		if (e == it.second) {
			arg.hints.emplace_back(it.first);
		}
	}

	// Check context list on top of the stack (most recent bound first)
	for (auto it = this->contexts.rbegin(); it != this->contexts.rend(); ++it) {
		if ((*it)->execute_if_bound(arg)) {
			return true;
		}
	}

	// If no local keybinds were bound, check the global keybinds
	return this->global_context.execute_if_bound(arg);
}


void InputManager::set_state(const Event &ev, bool is_down) {
	if (this->ignored(ev)) {
		return;
	}

	// update key states
	this->keymod = ev.mod;
	bool was_down = this->states[ev.cc];
	this->states[ev.cc] = is_down;

	// a key going from pressed to unpressed
	// will automatically trigger event handling
	if (was_down && !is_down) {
		this->trigger(ev);
	}
}


void InputManager::set_mouse(int x, int y) {
	coord::window last_position = this->mouse_position;
	this->mouse_position = coord::window {(coord::pixel_t) x, (coord::pixel_t) y};
	this->mouse_motion = this->mouse_position - last_position;
}


void InputManager::set_motion(int x, int y) {
	this->mouse_motion.x = x;
	this->mouse_motion.y = y;
}


void InputManager::set_relative(bool mode) {
	if (this->relative_mode == mode) {
		return;
	}

	// change mode
	this->relative_mode = mode;
	if (this->relative_mode) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	else {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
}


bool InputManager::is_down(const ClassCode &cc) const {
	auto it = this->states.find(cc);
	if (it != this->states.end()) {
		return it->second;
	}
	return false;
}


bool InputManager::is_down(event_class ec, code_t code) const {
	return is_down(ClassCode(ec, code));
}


bool InputManager::is_down(SDL_Keycode k) const {
	return is_down(sdl_key(k).cc);
}


bool InputManager::is_mod_down(modifier mod) const {
	return (this->keymod.count(mod) > 0);
}


modset_t InputManager::get_mod() const {
	SDL_Keymod mod = SDL_GetModState();
	return sdl_mod(mod);
}


bool InputManager::on_input(SDL_Event *e) {

	// top level input handler
	switch (e->type) {

	case SDL_KEYUP: {
		SDL_Keycode code = reinterpret_cast<SDL_KeyboardEvent *>(e)->keysym.sym;
		Event ev = sdl_key(code, SDL_GetModState());
		this->set_state(ev, false);
		break;
	} // case SDL_KEYUP

	case SDL_KEYDOWN: {
		SDL_Keycode code = reinterpret_cast<SDL_KeyboardEvent *>(e)->keysym.sym;
		this->set_state(sdl_key(code, SDL_GetModState()), true);
		break;
	} // case SDL_KEYDOWN

	case SDL_TEXTINPUT: {
		this->trigger(utf8(e->text.text));
		break;
	} // case SDL_TEXTINPUT

	case SDL_MOUSEBUTTONUP: {
		this->set_relative(false);
		this->trigger(sdl_mouse_up_down(e->button.button, true, SDL_GetModState()));
		Event ev = sdl_mouse(e->button.button, SDL_GetModState());
		this->set_state(ev, false);
		break;
	} // case SDL_MOUSEBUTTONUP

	case SDL_MOUSEBUTTONDOWN: {

		// TODO: set which buttons
		if (e->button.button == 2) {
			this->set_relative(true);
		}
		this->trigger(sdl_mouse_up_down(e->button.button, false, SDL_GetModState()));
		Event ev = sdl_mouse(e->button.button, SDL_GetModState());
		this->set_state(ev, true);
		break;
	} // case SDL_MOUSEBUTTONDOWN

	case SDL_MOUSEMOTION: {
		if (this->relative_mode) {
			this->set_motion(e->motion.xrel, e->motion.yrel);
		}
		else {
			this->set_mouse(e->button.x, e->button.y);
		}

		// must occur after setting mouse position
		Event ev(event_class::MOUSE_MOTION, 0, this->get_mod());
		this->trigger(ev);
		break;
	} // case SDL_MOUSEMOTION

	case SDL_MOUSEWHEEL: {
		Event ev = sdl_wheel(e->wheel.y, SDL_GetModState());
		this->trigger(ev);
		break;
	} // case SDL_MOUSEWHEEL

	} // switch (e->type)

	return true;
}


std::vector<std::string> InputManager::active_binds(const std::unordered_map<action_t, action_func_t> &ctx_actions) const {

	std::vector<std::string> result;

	// TODO: this only checks the by_type mappings, the others are missing!
	for (auto &action : ctx_actions) {
		std::string keyboard_key;

		for (auto &key : this->keys) {
			if (key.first == action.first) {
				keyboard_key = event_as_string(key.second);
				break;
			}
		}

		// this is only possible if the action is registered,
		// then this->input_manager != nullptr.
		// TODO: try to purge the action manager access here.
		std::string action_type_str = this->get_action_manager()->get_name(action.first);

		result.push_back(keyboard_key + " : " + action_type_str);
	}

	return result;
}


ActionManager *InputManager::get_action_manager() const {
	return this->action_manager;
}


}} // openage::input
