// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>

#include "time/time.h"


namespace openage::gamestate {
class GameEntity;

namespace activity {

/**
 * Condition for next command check in the activity system.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity.
 *
 * @return true if the entity has a idle command next in the queue, false otherwise.
 */
bool next_command_idle(const time::time_t &time,
                       const std::shared_ptr<gamestate::GameEntity> &entity);

/**
 * Condition for next command check in the activity system.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity.
 *
 * @return true if the entity has a move command next in the queue, false otherwise.
 */
bool next_command_move(const time::time_t &time,
                       const std::shared_ptr<gamestate::GameEntity> &entity);

} // namespace activity
} // namespace openage::gamestate
