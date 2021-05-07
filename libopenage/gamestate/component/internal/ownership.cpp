// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "ownership.h"
#include "../component_type.h"


namespace openage::gamestate::component {

Ownership::Ownership(const std::shared_ptr<event::Loop> &loop,
                     const uint64_t owner_id,
                     const time_t &creation_time) :
	owner(loop, 0) {
	this->owner.set_insert(creation_time, owner_id);
}

inline component_t Ownership::get_component_type() const {
	return component_t::OWNERSHIP;
}

} // namespace openage::gamestate::component
