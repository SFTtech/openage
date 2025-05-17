// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <nyan/nyan.h>

#include "gamestate/component/types.h"
#include "time/time.h"


namespace openage {

namespace gamestate {
class GameEntity;
class GameState;

namespace system {


class ApplyEffect {
public:
	/**
	 * Apply the effect of an ability from a command.
	 *
	 * @param entity Game entity applying the effects.
	 * @param state Game state.
	 * @param start_time Start time of change.
	 *
	 * @return Runtime of the change in simulation time.
	 */
	static const time::time_t apply_effect_command(const std::shared_ptr<gamestate::GameEntity> &entity,
	                                               const std::shared_ptr<openage::gamestate::GameState> &state,
	                                               const time::time_t &start_time);

	/**
	 * Apply the effect of an ability to a game entity.
	 *
	 * @param effector Game entity applying the effects.
	 * @param state Game state.
	 * @param resistor Target entity of the effects.
	 * @param start_time Start time of change.
	 *
	 * @return Runtime of the change in simulation time.
	 */
	static const time::time_t apply_effect(const std::shared_ptr<gamestate::GameEntity> &effector,
	                                       const std::shared_ptr<openage::gamestate::GameState> &state,
	                                       const std::shared_ptr<gamestate::GameEntity> &resistor,
	                                       const time::time_t &start_time);

private:
	/**
	 * Get the gross applied value for discrete FlatAttributeChange effects.
	 *
	 * The gross applied value is calculated as follows:
	 *
	 *    applied_value = clamp(change_value - block_value, min_change, max_change)
	 *
	 * Effects and resistances MUST have the same attribute change type.
	 *
	 * @param effects Effects of the effector.
	 * @param resistances Resistances of the resistor.
	 *
	 * @return Gross applied attribute change value.
	 */
	static const component::attribute_value_t get_applied_discrete_flac(const std::vector<nyan::Object> &effects,
	                                                                    const std::vector<nyan::Object> &resistances);
};

} // namespace system
} // namespace gamestate
} // namespace openage
