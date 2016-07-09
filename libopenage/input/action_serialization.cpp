// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "action_serialization.h"

#include <sstream>

#include "../coord/camgame.h"
#include "../coord/phys3.h"
#include "../coord/phys3_serialization.h"

#include "event_serialization.h"

namespace openage {
namespace input {

action_arg_t to_action(const deserialized_action_arg_t &action) {
	return action_arg_t{Event{action.e.cc.eclass, action.e.cc.code, action.e.mod}, action.mouse, action.motion, action.hints};
}

std::ostream& operator<<(std::ostream &o, std::tuple<const action_arg_t&, const ActionManager&> action) {
	auto &arg = std::get<0>(action);
	o << arg.e;
	o << arg.mouse.to_camgame().to_phys3();
	o << arg.motion.to_camgame().to_phys3();

	auto &manager = std::get<1>(action);
	for (auto hint : arg.hints)
		o << ' ' << manager.get_name(hint);

	return o;
}

std::istream& operator>>(std::istream &i, std::tuple<deserialized_action_arg_t&, const ActionManager&> action) {
	auto &arg = std::get<0>(action);

	coord::phys3 mouse;
	coord::phys3_delta motion;

	i >> arg.e >> mouse >> motion;

	arg.mouse = mouse.to_camgame().to_window();
	arg.motion = motion.to_camgame().to_window();

	std::string hints;
	std::getline(i, hints);
	std::istringstream hints_stream(hints);

	auto &manager = std::get<1>(action);
	for (std::string hint; hints_stream >> hint;)
		arg.hints.push_back(manager.get(hint));

	return i;
}

}} // openage::input
