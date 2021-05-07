// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "live.h"
#include "../component_type.h"


namespace openage::gamestate::component {

Live::Live(const std::shared_ptr<event::Loop> &loop,
           nyan::Object &ability,
           nyan::View &view,
           const time_t &creation_time,
           const bool enabled) :
	APIComponent(loop, ability, creation_time, enabled),
	attribute_values{} {
	for (nyan::ValueHolder attr_setting : this->ability.get_set("attributes")) {
		nyan::ObjectValue setting_val{
			*std::dynamic_pointer_cast<nyan::ObjectValue>(attr_setting.get_ptr())};
		nyan::fqon_t setting_fqon{setting_val.get()};
		nyan::Object setting{view.get_object(setting_fqon)};

		nyan::fqon_t attribute_fqon{setting.get_object("attribute").get_name()};
		auto starting_value{setting.get_int("starting_value")};

		auto attr_curve{std::make_shared<curve::Discrete<uint64_t>>(loop, 0)};
		attr_curve->set_insert(creation_time, starting_value);

		this->attribute_values.insert(creation_time, attribute_fqon, attr_curve);
	}
}

component_t Live::get_component_type() const {
	return component_t::LIVE;
}

} // namespace openage::gamestate::component
