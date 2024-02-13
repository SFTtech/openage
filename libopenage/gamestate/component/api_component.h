// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <nyan/nyan.h>

#include "curve/discrete.h"
#include "gamestate/component/base_component.h"
#include "time/time.h"


namespace openage {

namespace event {
class EventLoop;
}

namespace gamestate::component {

/**
 * Interface for componenta that are represented by a nyan object from
 * the engine.ability namespace in the openage modding API.
 */
class APIComponent : public Component {
public:
	/**
	 * Creates an APIComponent.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 * @param ability nyan ability object for the component.
	 * @param creation_time Ingame creation time of the component.
	 * @param enabled If true, enable the component at creation time.
	 */
	APIComponent(const std::shared_ptr<openage::event::EventLoop> &loop,
	             nyan::Object &ability,
	             const time::time_t &creation_time,
	             bool enabled = true);

	/**
	 * Creates an APIComponent.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 * @param ability nyan ability object for the component.
	 * @param enabled If true, enable the component at creation time.
	 */
	APIComponent(const std::shared_ptr<openage::event::EventLoop> &loop,
	             nyan::Object &ability,
	             bool enabled = true);

	/**
	 * Get the ability object from the nyan dataset.
	 *
	 * @return Ability object.
	 */
	const nyan::Object &get_ability() const;

private:
	/**
	 * nyan object holding the data for the component.
	 */
	nyan::Object ability;

	/**
	 * Determines if the component is available to its game entity.
	 */
	curve::Discrete<bool> enabled;
};

} // namespace gamestate::component
} // namespace openage
