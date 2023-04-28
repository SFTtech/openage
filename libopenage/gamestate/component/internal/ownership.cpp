// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "ownership.h"
#include "../component_type.h"


namespace openage::gamestate::component {

Ownership::Ownership(const std::shared_ptr<openage::event::Loop> &loop,
                     const ownership_id_t owner_id,
                     const curve::time_t &creation_time) :
	owner(loop, 0) {
	this->owner.set_insert(creation_time, owner_id);
}

inline component_t Ownership::get_component_type() const {
	return component_t::OWNERSHIP;
}

} // namespace openage::gamestate::component
