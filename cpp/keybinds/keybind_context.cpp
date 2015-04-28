// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "keybind_context.h"

namespace openage {
namespace keybinds {

KeybindContext::KeybindContext() {}


void KeybindContext::bind(const action_t a, const std::function<void()> f) {
	this->binds[a] = f;
}


bool KeybindContext::execute_if_bound(action_t a) {
	auto f = this->binds.find(a);
	if (f != this->binds.end()) {
		f->second();
		return true;
	}
	return false;
}


} //namespace keybinds
} //namespace openage
