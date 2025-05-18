// Copyright 2024-2025 the openage authors. See copying.md for legal info.

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
 * Check the type of the next command in the queue and return its associated key
 * value.
 *
 * The key value is the result of a static_cast of the \p command_t enum value.
 *
 * @param time Time when the condition is checked.
 * @param entity Game entity that the activity is assigned to.
 *
 * @return Key of the output node.
 */
int next_command_switch(const time::time_t &time,
                        const std::shared_ptr<gamestate::GameEntity> &entity,
                        const std::shared_ptr<gamestate::GameState> & /* state */,
                        const std::shared_ptr<nyan::Object> & /* api_object */);

} // namespace activity
} // namespace openage::gamestate
