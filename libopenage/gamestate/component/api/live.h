// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <nyan/nyan.h>

#include "curve/continuous.h"
#include "curve/map.h"
#include "gamestate/component/api_component.h"

namespace openage::gamestate::component {
class Live : public APIComponent {
public:
	/**
	 * Creates a Live component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 * @param ability nyan ability object for the component.
	 * @param view nyan view of the owner of the component's entity.
	 * @param creation_time Ingame creation time of the component.
	 * @param enabled If true, enable the component at creation time.
	 */
	Live(const std::shared_ptr<event::EventLoop> &loop,
	     nyan::Object &ability,
	     nyan::View &view,
	     const time_t &creation_time,
	     bool enabled = true);

	/**
	 * Creates a Live component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 * @param ability nyan ability object for the component.
	 * @param enabled If true, enable the component at creation time.
	 */
	Live(const std::shared_ptr<event::EventLoop> &loop,
	     nyan::Object &ability,
	     bool enabled = true);

	component_t get_type() const override;

private:
	/**
	 * Map of attribute values by attribute type.
	 */
	curve::UnorderedMap<nyan::fqon_t,
	                    std::shared_ptr<curve::Discrete<uint64_t>>>
		attribute_values;
};

} // namespace openage::gamestate::component
