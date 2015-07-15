// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <functional>

#include "action.h"

namespace openage {
namespace input {

action_arg_t::action_arg_t(Event e, coord::window m, coord::window_delta mm)
	:
	e(e),
	mouse(m),
	motion(mm) {
}

Action::Action(const action_func_t f)
	:
	Action(action_t::UNDEFINED, f) {}


Action::Action(const action_id_t &type, const action_func_t f)
	:
	type(type),
	on_action(f) {
}


bool Action::do_action(const action_arg_t &arg) {
	this->on_action(arg);
	return true;
}


int action_hash::operator()(const action_t& a) const {
	return std::hash<int>()(static_cast<int>(a));
}


} //namespace input
} //namespace openage
