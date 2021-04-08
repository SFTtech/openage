// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once

#include <nyan/nyan.h>

#include "../../curve/continuous.h"
#include "../../curve/map.h"
#include "../api_component.h"

namespace openage::gamestate::component {

class Live : APIComponent {
	curve::UnorderedMap<nyan::Object,
	                    std::unique_ptr<curve::Continuous<uint64_t>>>
		attribute_values;

	Live(nyan::Object &ability,
	     const bool enabled,
	     const std::shared_ptr<event::Loop> &loop);
};

} // namespace openage::gamestate::component
