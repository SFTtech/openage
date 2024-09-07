// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <memory>

#include "curve/discrete.h"
#include "gamestate/component/internal_component.h"
#include "gamestate/component/types.h"
#include "gamestate/types.h"
#include "time/time.h"


namespace openage {
namespace event {
class EventLoop;
}

namespace gamestate::component {

class Ownership final : public InternalComponent {
public:
	/**
	 * Creates an Ownership component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 * @param owner_id Initial owner ID at creation time.
	 * @param creation_time Ingame creation time of the component.
	 */
	Ownership(const std::shared_ptr<openage::event::EventLoop> &loop,
	          const player_id_t owner_id,
	          const time::time_t &creation_time);

	/**
	 * Creates an Ownership component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 */
	Ownership(const std::shared_ptr<openage::event::EventLoop> &loop);

	component_t get_type() const override;

	/**
	 * Set the owner ID at a given time.
	 *
	 * @param time Time at which the owner ID is set.
	 * @param owner_id New owner ID.
	 */
	void set_owner(const time::time_t &time, const player_id_t owner_id);

	/**
	 * Get the owner IDs over time.
	 *
	 * @return Owner ID curve.
	 */
	const curve::Discrete<player_id_t> &get_owners() const;

private:
	/**
	 * Owner ID storage over time.
	 */
	curve::Discrete<player_id_t> owner;
};

} // namespace gamestate::component
} // namespace openage
