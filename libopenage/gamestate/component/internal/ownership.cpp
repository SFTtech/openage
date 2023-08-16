// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "ownership.h"

#include "gamestate/component/types.h"


namespace openage::gamestate::component {

Ownership::Ownership(const std::shared_ptr<openage::event::EventLoop> &loop,
                     const player_id_t owner_id,
                     const time::time_t &creation_time) :
	owner(loop, 0) {
	this->owner.set_last(creation_time, owner_id);
}

Ownership::Ownership(const std::shared_ptr<openage::event::EventLoop> &loop) :
	owner(loop, 0) {
}

inline component_t Ownership::get_type() const {
	return component_t::OWNERSHIP;
}

void Ownership::set_owner(const time::time_t &time, const player_id_t owner_id) {
	this->owner.set_last(time, owner_id);
}

const curve::Discrete<player_id_t> &Ownership::get_owners() const {
	return this->owner;
}

} // namespace openage::gamestate::component
