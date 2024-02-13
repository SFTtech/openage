// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate.h"

#include <vector>

namespace openage::event::demo {

std::vector<PongEvent> get_ai_inputs(const std::shared_ptr<PongPlayer> &player,
                                     const std::shared_ptr<PongBall> &ball,
                                     const time::time_t &now);

} // namespace openage::event::demo
