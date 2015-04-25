// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "keybind_manager.h"

#include <array>

namespace openage {
namespace keybinds {

KeybindManager::KeybindManager()
	: // TODO get this from a file instead of hardcoding it here
	keys {{ key_t(SDLK_ESCAPE), action_t::STOP_GAME },
	      { key_t(SDLK_F1), action_t::TOGGLE_HUD },
	      { key_t(SDLK_F2), action_t::SCREENSHOT },
	      { key_t(SDLK_F3), action_t::TOGGLE_DEBUG_OVERLAY },
	      { key_t(SDLK_F4), action_t::TOGGLE_DEBUG_GRID },
	      { key_t(SDLK_F5), action_t::QUICK_SAVE },
	      { key_t(SDLK_F9), action_t::QUICK_LOAD },
	      { key_t(SDLK_SPACE), action_t::TOGGLE_BLENDING },
	      { key_t(SDLK_F12), action_t::TOGGLE_PROFILER },
	      { key_t(SDLK_m), action_t::TOGGLE_CONSTRUCT_MODE },
	      { key_t(SDLK_p), action_t::TOGGLE_UNIT_DEBUG },
	      { key_t(SDLK_t), action_t::TRAIN_OBJECT },
	      { key_t(SDLK_y), action_t::ENABLE_BUILDING_PLACEMENT },
	      { key_t(SDLK_z), action_t::DISABLE_SET_ABILITY },
	      { key_t(SDLK_x), action_t::SET_ABILITY_MOVE },
	      { key_t(SDLK_c), action_t::SET_ABILITY_GATHER },
	      { key_t(SDLK_BACKQUOTE), action_t::TOGGLE_CONSOLE},
	      { key_t(SDLK_v), action_t::SPAWN_VILLAGER},
	      { key_t(SDLK_DELETE), action_t::KILL_UNIT}},
	keymod{KMOD_NONE} {

}


KeybindContext &KeybindManager::get_global_keybind_context() {
	return this->global_hotkeys;
}


void KeybindManager::override_context(KeybindContext *context) {
	// Create a new context list, unless no context lists exist
	if (!this->contexts.empty()) {
		this->contexts.push(std::vector<KeybindContext *>());
	}
	this->contexts.top().push_back(context);
}


void KeybindManager::register_context(KeybindContext *context) {
	// Create a context list if none exist
	if (this->contexts.empty()) {
		this->contexts.push(std::vector<KeybindContext *>());
	}
	this->contexts.top().push_back(context);
}


void KeybindManager::remove_context() {
	auto top = this->contexts.top();
	top.pop_back();
	if (top.size() == 0) {
		this->contexts.pop();
	}
}


void KeybindManager::press(key_t k) {
	// Remove modifiers like num lock and caps lock
	k.mod = static_cast<SDL_Keymod>(k.mod & this->used_keymods);

	// Check whether key combination is bound to an action
	auto a = this->keys.find(k);
	if (a == this->keys.end()) {
		return;
	}
	action_t action = a->second;

	if (!this->contexts.empty()) {
		// Check context list on top of the stack (most recent bound first)
		for (auto it = this->contexts.top().rbegin(); it != this->contexts.top().rend(); ++it) {
			if ((*it)->execute_if_bound(action)) {
				return;
			}
		}
	}

	// If no local keybinds were bound, check the global keybinds
	this->global_hotkeys.execute_if_bound(action);
}


void KeybindManager::set_key_state(SDL_Keycode k, SDL_Keymod mod, bool is_down) {
	this->keymod = mod;
	this->key_states[k] = is_down;
}


// not that the function stores a unknown/new keycode
// as 'not pressed' when it was requested
bool KeybindManager::is_key_down(SDL_Keycode k) {
	auto it = this->key_states.find(k);
	if (it != this->key_states.end()) {
		return it->second;
	} else {
		this->key_states[k] = false;
		return false;
	}
}

bool KeybindManager::is_keymod_down(SDL_Keymod mod) const {
	return (this->keymod & mod) == mod;
}


} //namespace keybinds
} //namespace openage
