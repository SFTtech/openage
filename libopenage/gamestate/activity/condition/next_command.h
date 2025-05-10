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
 * Check whether the next command in the entity command queue is of type \p IDLE.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity that the activity is assigned to.
 *
 * @return true if the entity has a \p IDLE command next in the queue, false otherwise.
 */
bool next_command_idle(const time::time_t &time,
                       const std::shared_ptr<gamestate::GameEntity> &entity,
                       const std::shared_ptr<nyan::Object> & /* api_object */);

/**
 * Check whether the next command in the entity command queue is of type \p MOVE.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity that the activity is assigned to.
 *
 * @return true if the entity has a \p MOVE command next in the queue, false otherwise.
 */
bool next_command_move(const time::time_t &time,
                       const std::shared_ptr<gamestate::GameEntity> &entity,
                       const std::shared_ptr<nyan::Object> & /* api_object */);

/**
 * Check whether the next command in the entity command queue is of type \p APPLY_EFFECT.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity that the activity is assigned to.
 *
 * @return true if the entity has an \p APPLY_EFFECT command next in the queue, false otherwise.
 */
bool next_command_apply_effect(const time::time_t &time,
                               const std::shared_ptr<gamestate::GameEntity> &entity,
                               const std::shared_ptr<nyan::Object> & /* api_object */);

} // namespace activity
} // namespace openage::gamestate
