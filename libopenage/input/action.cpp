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


int action_hash::operator()(const action_t& a) const {
	return std::hash<int>()(static_cast<int>(a));
}


} //namespace input
} //namespace openage
