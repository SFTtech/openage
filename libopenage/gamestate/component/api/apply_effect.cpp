// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "apply_effect.h"


namespace openage::gamestate::component {

ApplyEffect::ApplyEffect(const std::shared_ptr<openage::event::EventLoop> &loop,
                         nyan::Object &ability,
                         const time::time_t &creation_time,
                         bool enabled) :
	APIComponent{loop, ability, creation_time, enabled},
	init_time{loop, 0},
	last_used{loop, 0} {
}

ApplyEffect::ApplyEffect(const std::shared_ptr<openage::event::EventLoop> &loop,
                         nyan::Object &ability,
                         bool enabled) :
	APIComponent{loop, ability, enabled},
	init_time{loop, 0},
	last_used{loop, 0} {
}

component_t ApplyEffect::get_type() const {
	return component_t::APPLY_EFFECT;
}

const curve::Discrete<time::time_t> &ApplyEffect::get_init_time() const {
	return this->init_time;
}

const curve::Discrete<time::time_t> &ApplyEffect::get_last_used() const {
	return this->last_used;
}

void ApplyEffect::set_init_time(const time::time_t &time) {
	this->init_time.set_last(time, time);
}

void ApplyEffect::set_last_used(const time::time_t &time) {
	this->last_used.set_last(time, time);
}

} // namespace openage::gamestate::component
