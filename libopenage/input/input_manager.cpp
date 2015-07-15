// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <algorithm>
#include <array>

#include "../log/log.h"
#include "../engine.h"
#include "input_manager.h"

namespace openage {
namespace input {

InputManager::InputManager()
	: // TODO get this from a file instead of hardcoding it here
	keys {{ sdl_key(SDLK_RETURN), action_t::START_GAME },
	      { sdl_key(SDLK_ESCAPE), action_t::STOP_GAME },
	      { sdl_key(SDLK_F1), action_t::TOGGLE_HUD },
	      { sdl_key(SDLK_F2), action_t::SCREENSHOT },
	      { sdl_key(SDLK_F3), action_t::TOGGLE_DEBUG_OVERLAY },
	      { sdl_key(SDLK_F4), action_t::TOGGLE_DEBUG_GRID },
	      { sdl_key(SDLK_F5), action_t::QUICK_SAVE },
	      { sdl_key(SDLK_F9), action_t::QUICK_LOAD },
	      { sdl_key(SDLK_SPACE), action_t::TOGGLE_BLENDING },
	      { sdl_key(SDLK_F12), action_t::TOGGLE_PROFILER },
	      { sdl_key(SDLK_m), action_t::TOGGLE_CONSTRUCT_MODE },
	      { sdl_key(SDLK_p), action_t::TOGGLE_UNIT_DEBUG },
	      { sdl_key(SDLK_t), action_t::TRAIN_OBJECT },
	      { sdl_key(SDLK_y), action_t::ENABLE_BUILDING_PLACEMENT },
	      { sdl_key(SDLK_z, KMOD_LCTRL), action_t::DISABLE_SET_ABILITY },
	      { sdl_key(SDLK_x, KMOD_LCTRL), action_t::SET_ABILITY_MOVE },
	      { sdl_key(SDLK_c, KMOD_LCTRL), action_t::SET_ABILITY_GATHER },
	      { sdl_key(SDLK_BACKQUOTE), action_t::TOGGLE_CONSOLE},
	      { sdl_key(SDLK_v), action_t::SPAWN_VILLAGER},
	      { sdl_key(SDLK_DELETE), action_t::KILL_UNIT},
	      { sdl_key(SDLK_q), action_t::BUILDING_1},
	      { sdl_key(SDLK_w), action_t::BUILDING_2},
	      { sdl_key(SDLK_e), action_t::BUILDING_3},
	      { sdl_key(SDLK_r), action_t::BUILDING_4},
	      { sdl_key(SDLK_q, KMOD_LCTRL), action_t::BUILDING_1},
	      { sdl_key(SDLK_w, KMOD_LCTRL), action_t::BUILDING_2},
	      { sdl_key(SDLK_e, KMOD_LCTRL), action_t::BUILDING_3},
	      { sdl_key(SDLK_r, KMOD_LCTRL), action_t::BUILDING_4},
	      { sdl_key(SDLK_z), action_t::BUILDING_TOWN_CENTER},
	      { sdl_key(SDLK_1), action_t::SWITCH_TO_PLAYER_1},
	      { sdl_key(SDLK_2), action_t::SWITCH_TO_PLAYER_2},
	      { sdl_key(SDLK_3), action_t::SWITCH_TO_PLAYER_3},
	      { sdl_key(SDLK_4), action_t::SWITCH_TO_PLAYER_4},
	      { sdl_key(SDLK_5), action_t::SWITCH_TO_PLAYER_5},
	      { sdl_key(SDLK_6), action_t::SWITCH_TO_PLAYER_6},
	      { sdl_key(SDLK_7), action_t::SWITCH_TO_PLAYER_7},
	      { sdl_key(SDLK_8), action_t::SWITCH_TO_PLAYER_8},
	      { sdl_mouse(1), action_t::PAINT_TERRAIN},
	      { sdl_mouse(1), action_t::SELECT},
	      { sdl_mouse(3), action_t::DESELECT},
	      { sdl_wheel(1), action_t::FORWARD},
	      { sdl_wheel(-1), action_t::BACK}} {
}


InputContext &InputManager::get_global_context() {
	return this->global_hotkeys;
}


void InputManager::override_context(InputContext *context) {
	// Create a new context list, unless no context lists exist
	if (!this->contexts.empty()) {
		this->contexts.push(std::vector<InputContext *>());
	}
	this->contexts.top().push_back(context);
}


void InputManager::register_context(InputContext *context) {
	// Create a context list if none exist
	if (this->contexts.empty()) {
		this->contexts.push(std::vector<InputContext *>());
	}
	this->contexts.top().push_back(context);
}


void InputManager::remove_context() {
	if (this->contexts.empty()) {
		return;
	}
	auto top = this->contexts.top();
	top.pop_back();
	if (top.size() == 0) {
		this->contexts.pop();
	}
}


void InputManager::trigger(const Event &e) {
	// arg passed to recievers
	action_arg_t arg(e, this->mouse_position, this->mouse_motion);

	// Check whether key combination is bound to an action
	auto range = keys.equal_range(e);
	if (range.first != keys.end()) {
		std::for_each(range.first, range.second, [&arg](binding_map_t::value_type &v) {
			arg.hints.push_back(v.second);
		});
	}

	if (!this->contexts.empty()) {
		// Check context list on top of the stack (most recent bound first)
		for (auto it = this->contexts.top().rbegin(); it != this->contexts.top().rend(); ++it) {
			if ((*it)->execute_if_bound(arg)) {
				return;
			}
		}
	}

	// If no local keybinds were bound, check the global keybinds
	this->global_hotkeys.execute_if_bound(arg);
}


void InputManager::set_state(const Event &ev, bool is_down) {
	this->keymod = ev.mod;
	this->states[ev.cc] = is_down;

	if (!is_down) {
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

bool InputManager::is_down(class_code_t cc) const {
	auto it = this->states.find(cc);
	if (it != this->states.end()) {
		return it->second;
	}
	return false;
}


bool InputManager::is_down(event_class ec, code_t code) const {
	return is_down(class_code_t(ec, code));
}


bool InputManager::is_down(SDL_Keycode k) const {
	return is_down(sdl_key(k).cc);
}


bool InputManager::is_mod_down(SDL_Keymod mod) const {
	return false; //this->keymod == sdl_mod(mod);
}


modset_t InputManager::get_mod() const {
	SDL_Keymod mod = SDL_GetModState();
	return sdl_mod(mod);
}


bool InputManager::on_input(SDL_Event *e) {
	Engine &engine = Engine::get();

	// top level input handler
	switch (e->type) {

	case SDL_KEYUP: {
		SDL_Keycode code = reinterpret_cast<SDL_KeyboardEvent *>(e)->keysym.sym;
		Event ev(event_class::KEYBOARD, code, this->get_mod());
		this->set_state(ev, false);
		break;
	} // case SDL_KEYUP

	case SDL_KEYDOWN: {
		SDL_Keycode code = reinterpret_cast<SDL_KeyboardEvent *>(e)->keysym.sym;
		Event ev(event_class::KEYBOARD, code, this->get_mod());
		this->set_state(ev, true);
		break;
	} // case SDL_KEYDOWN

	case SDL_MOUSEBUTTONUP: {
		this->set_relative(false);
		Event ev(event_class::MOUSE_BUTTON, e->button.button, this->get_mod());
		this->set_state(ev, false);
		break;
	} // case SDL_MOUSEBUTTONUP

	case SDL_MOUSEBUTTONDOWN: {

		// TODO: set which buttons
		if (e->button.button == 2) {
			this->set_relative(true);
		}
		Event ev(event_class::MOUSE_BUTTON, e->button.button, this->get_mod());
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
		this->set_state(ev, false);
		break;
	} // case SDL_MOUSEMOTION

	case SDL_MOUSEWHEEL: {
		Event ev(event_class::MOUSE_WHEEL, e->wheel.y, this->get_mod());
		this->set_state(ev, false);
		break;
	} // case SDL_MOUSEWHEEL

	} // switch (e->type)

	return true;
}


} //namespace input
} //namespace openage
