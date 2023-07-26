// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "api_component.h"


namespace openage::gamestate::component {

APIComponent::APIComponent(const std::shared_ptr<event::EventLoop> &loop,
                           nyan::Object &ability,
                           const time::time_t &creation_time,
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

} // namespace openage::gamestate::component
