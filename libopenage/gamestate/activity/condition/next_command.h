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
 * Check whether the next command in the entity command queue is of a specific type.
 *
 * The command type is parsed from the nyan object \p condition.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity that the activity is assigned to.
 * @param condition nyan object for the condition. Used to read the command type.
 *
 * @return true if the entity has the command next in the queue, false otherwise.
 */
bool next_command(const time::time_t &time,
                  const std::shared_ptr<gamestate::GameEntity> &entity,
                  const std::shared_ptr<nyan::Object> &condition);

} // namespace activity
} // namespace openage::gamestate
