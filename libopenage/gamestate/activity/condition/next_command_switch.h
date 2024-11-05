// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>

#include "time/time.h"


namespace openage::gamestate {
class GameEntity;

namespace activity {

/**
 * Returns true if the next command in the queue is an idle command.
 */
int next_command_switch(const time::time_t &time,
                        const std::shared_ptr<gamestate::GameEntity> &entity);

} // namespace activity
} // namespace openage::gamestate
