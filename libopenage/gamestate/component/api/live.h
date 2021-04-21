// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once

#include <nyan/nyan.h>

#include "../../../curve/continuous.h"
#include "../../../curve/map.h"
#include "../api_component.h"

namespace openage::gamestate::component {
class Live : APIComponent {
	/**
	 * Map of attribute values by attribute type.
	 */
	curve::UnorderedMap<nyan::fqon_t,
	                    std::shared_ptr<curve::Discrete<uint64_t>>>
		attribute_values;

	Live(const std::shared_ptr<event::Loop> &loop,
	     nyan::Object &ability,
	     nyan::View &view,
	     const time_t &creation_time,
	     const bool enabled);

	component_t get_component_type() const override;
};

} // namespace openage::gamestate::component
