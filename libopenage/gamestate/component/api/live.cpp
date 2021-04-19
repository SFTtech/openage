// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "live.h"
#include "../component_type.h"


namespace openage::gamestate::component {

Live::Live(const std::shared_ptr<event::Loop> &loop,
           nyan::Object &ability,
           const time_t &creation_time,
           const bool enabled) :
	APIComponent(loop, ability, creation_time, enabled),
	attribute_values{} {
	for (auto &attr_setting : this->ability.get_set("attributes")) {
		// TODO: Creates curves for attribute setttings
	}
}

component_t Live::get_component_type() const {
	return component_t::LIVE;
}

} // namespace openage::gamestate::component
