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
 * Check whether the entity has a component enabled matching the ability from the
 * given API object.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity that the activity is assigned to.
 * @param condition nyan object for the condition. Used to read the ability reference.
 *
 * @return true if there is at least one component enabled matching the ability, false otherwise.
 */
bool component_enabled(const time::time_t &time,
                       const std::shared_ptr<gamestate::GameEntity> &entity,
                       const std::shared_ptr<gamestate::GameState> & /* state */,
                       const std::shared_ptr<nyan::Object> &condition);

} // namespace activity
} // namespace openage::gamestate
