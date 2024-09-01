// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "curve/discrete.h"
#include "gamestate/component/api_component.h"
#include "gamestate/component/types.h"
#include "time/time.h"


namespace openage::gamestate::component {

/**
 * Component for ApplyEffect abilities.
 */
class ApplyEffect final : public APIComponent {
public:
	ApplyEffect(const std::shared_ptr<openage::event::EventLoop> &loop,
	            nyan::Object &ability,
	            const time::time_t &creation_time,
	            bool enabled = true);

	ApplyEffect(const std::shared_ptr<openage::event::EventLoop> &loop,
	            nyan::Object &ability,
	            bool enabled = true);

	component_t get_type() const override;

	/**
	 * Get the last initiaton time that is before the given \p time.
	 *
	 * @param time Current simulation time.
	 *
	 * @return Curve with the last initiation times.
	 */
	const curve::Discrete<time::time_t> &get_init_time() const;

	/**
	 * Get the last time the effects were applied before the given \p time.
	 *
	 * @param time Current simulation time.
	 *
	 * @return Curve with the last application times.
	 */
	const curve::Discrete<time::time_t> &get_last_used() const;

	/**
	 * Record the simulation time when the entity starts using the ability.
	 *
	 * @param time Time at which the entity initiates using the ability.
	 */
	void set_init_time(const time::time_t &time);

	/**
	 * Record the simulation time when the entity last applied the effects.
	 *
	 * @param time Time at which the entity last applied the effects.
	 */
	void set_last_used(const time::time_t &time);

private:
	/**
	 * Simulation time when the entity starts using the corresponding ability
	 * of the component. For example, when a unit starts attacking.
	 *
	 * Effects are applied after \p init_time + \p application_delay (from the nyan object).
	 *
	 * The curve stores the time both as the keyframe time AND the keyframe value. In
	 * practice, only the value should be used.
	 */
	curve::Discrete<time::time_t> init_time;

	/**
	 * Simulation time when the effects were applied last.
	 *
	 * Effects can only be applied again after a cooldown has passed, i.e.
	 * at \p last_used + \p reload_time (from the nyan object).
	 *
	 * The curve stores the time both as the keyframe time AND the keyframe value. In
	 * practice, only the value should be used.
	 */
	curve::Discrete<time::time_t> last_used;
};

} // namespace openage::gamestate::component
