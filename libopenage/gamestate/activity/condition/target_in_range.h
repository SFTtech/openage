// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>

#include "time/time.h"


namespace nyan {
class Object;
}

namespace openage::gamestate {
class GameEntity;
class GameState;

namespace activity {

/**
 * Check whether the current target of the game entity is within range of a specific ability.
 *
 * The ability type is parsed from the nyan object \p api_object.
 *
 * For the check to pass, the following subconditions must be met:
 * - \p entity has a target.
 * - \p entity has a matching component for the ability type.
 * - the distance between \p entity position and the target position is less than
 *   or equal to the range of the ability.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity that the activity is assigned to.
 * @param api_object nyan object for the condition. Used to read the command type.
 *
 * @return true if the target is within range of the ability, false otherwise.
 */
bool target_in_range(const time::time_t &time,
                     const std::shared_ptr<gamestate::GameEntity> &entity,
                     const std::shared_ptr<gamestate::GameState> &state,
                     const std::shared_ptr<nyan::Object> &api_object);

} // namespace activity
} // namespace openage::gamestate
