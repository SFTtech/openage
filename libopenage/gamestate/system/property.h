// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <nyan/nyan.h>

#include "time/time.h"


namespace openage::gamestate {
class GameEntity;

namespace system {

/**
 * Handle the animated property of an ability.
 *
 * @param entity Game entity.
 * @param ability Ability object.
 * @param start_time Start time of the animation.
 *
 * @return true if the ability has the property, false otherwise.
 */
bool handle_animated(const std::shared_ptr<gamestate::GameEntity> &entity,
                     const nyan::Object &ability,
                     const time::time_t &start_time);

} // namespace system
} // namespace openage::gamestate
