// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>

#include "time/time.h"


namespace nyan {
class Object;
}

namespace openage::gamestate {
class GameEntity;

namespace activity {

/**
 * Check whether the entity has a command in its command queue.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity that the activity is assigned to.
 *
 * @return true if there is at least one command in the entity's command queue, false otherwise.
 */
bool command_in_queue(const time::time_t &time,
                      const std::shared_ptr<gamestate::GameEntity> &entity,
                      const std::shared_ptr<nyan::Object> & /* api_object */);

} // namespace activity
} // namespace openage::gamestate
