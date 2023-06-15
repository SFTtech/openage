// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "api_component.h"

#include "gamestate/component/definitions.h"

namespace openage::gamestate::component {

APIComponent::APIComponent(const std::shared_ptr<event::EventLoop> &loop,
                           nyan::Object &ability,
                           const time_t &creation_time,
                           const bool enabled) :
	ability{ability},
	enabled(loop, 0) {
	this->enabled.set_insert(creation_time, enabled);
}

APIComponent::APIComponent(const std::shared_ptr<event::EventLoop> &loop,
                           nyan::Object &ability,
                           bool enabled) :
	ability{ability},
	enabled(loop, 0, "", nullptr, enabled) {
}

const nyan::Object &APIComponent::get_ability() const {
	return this->ability;
}

bool APIComponent::check_property(property_t property) {
	auto properties = this->ability.get<nyan::Dict>("Ability.properties");
	auto property_type = PROPERTY_DEFS.get(property);

	if (properties->contains(property_type)) {
		return true;
	}

	return false;
}

const nyan::Object APIComponent::get_property(property_t property) const {
	auto properties = this->ability.get<nyan::Dict>("Ability.properties");
	auto property_type = PROPERTY_DEFS.get(property);

	auto db_view = this->ability.get_view();
	auto property_val = std::dynamic_pointer_cast<nyan::ObjectValue>(properties->get().at(property_type).get_ptr());

	return db_view->get_object(property_val->get_name());
}


} // namespace openage::gamestate::component
