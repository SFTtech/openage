// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "curve/discrete.h"
#include "gamestate/component/api_component.h"
#include "gamestate/component/types.h"
#include "time/time.h"


namespace openage::gamestate::component {

/**
 * Stores runtime information for an ApplyEffect ability of a game entity.
 */
class ApplyEffect final : public APIComponent {
public:
	/**
	 * Creates an ApplyEffect component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 * @param ability nyan ability object for the component.
	 * @param creation_time Ingame creation time of the component.
	 * @param enabled If true, enable the component at creation time.
	 */
	ApplyEffect(const std::shared_ptr<openage::event::EventLoop> &loop,
	            nyan::Object &ability,
	            const time::time_t &creation_time,
	            bool enabled = true);

	/**
	 * Creates an ApplyEffect component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 * @param ability nyan ability object for the component.
	 * @param enabled If true, enable the component at creation time.
	 */
	ApplyEffect(const std::shared_ptr<openage::event::EventLoop> &loop,
	            nyan::Object &ability,
	            bool enabled = true);

	component_t get_type() const override;

	/**
	 * Get the last time an effect application was initiated that is before the given \p time.
	 *
	 * This should be used to determine if the application of the effect is still
	 * active and when the next application can be initiated.
	 *
	 * @param time Simulation time.
	 *
	 * @return Curve with the last initiation times.
	 */
	const curve::Discrete<time::time_t> &get_init_time() const;

	/**
	 * Get the last time the effects were applied before the given \p time.
	 *
	 * This should be used to determine if the effects are under a cooldown, i.e.
	 * to check if the effects can be applied again.
	 *
	 * @param time Simulation time.
	 *
	 * @return Curve with the last application times.
	 */
	const curve::Discrete<time::time_t> &get_last_used() const;

	/**
	 * Record the simulation time when the entity initiates an effect application,
	 * i.e. it starts using the ability.
	 *
	 * @param time Time at which the entity starts using the ability.
	 */
	void set_init_time(const time::time_t &time);

	/**
	 * Record the simulation time when the entity applies the effects
	 * of the ability, i.e. init time + application delay.
	 *
	 * @param time Time at which the entity applies the effects.
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
