// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>

#include "time/time.h"


namespace openage::gamestate {
class GameEntity;

namespace activity {

/**
 * Condition for command in queue check in the activity system.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity.
 *
 * @return true if there is at least one command in the entity's command queue, false otherwise.
 */
bool command_in_queue(const time::time_t &time,
                      const std::shared_ptr<gamestate::GameEntity> &entity);

} // namespace activity
} // namespace openage::gamestate
