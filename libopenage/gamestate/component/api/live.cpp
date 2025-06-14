// Copyright 2021-2025 the openage authors. See copying.md for legal info.

#include "live.h"

#include "curve/container/iterator.h"
#include "curve/container/map_filter_iterator.h"
#include "curve/segmented.h"
#include "gamestate/component/types.h"


namespace openage::gamestate::component {

component_t Live::get_type() const {
	return component_t::LIVE;
}

void Live::add_attribute(const time::time_t &time,
                         const nyan::fqon_t &attribute,
                         std::shared_ptr<curve::Segmented<attribute_value_t>> starting_values) {
	this->attributes.insert(time, attribute, starting_values);
}

const attribute_value_t Live::get_attribute(const time::time_t &time,
                                            const nyan::fqon_t &attribute) const {
	auto attribute_iterator = this->attributes.at(time, attribute);
	if (attribute_iterator) {
		auto attribute_curve = **attribute_iterator;
		return attribute_curve->get(time);
	}
	else {
		throw Error(MSG(err) << "Attribute not found: " << attribute);
	}
}

void Live::set_attribute(const time::time_t &time,
                         const nyan::fqon_t &attribute,
                         attribute_value_t value) {
	auto attribute_iterator = this->attributes.at(time, attribute);
	if (attribute_iterator) {
		auto attribute_curve = **attribute_iterator;
		auto current_value = attribute_curve->get(time);
		attribute_curve->set_last_jump(time, current_value, value);
	}
	else {
		throw Error(MSG(err) << "Attribute not found: " << attribute);
	}
}

} // namespace openage::gamestate::component
