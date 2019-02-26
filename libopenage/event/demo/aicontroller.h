// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate.h"

#include <vector>

namespace openage::event::demo {

std::vector<PongEvent> get_ai_inputs(const std::shared_ptr<PongPlayer> &player,
                                     const std::shared_ptr<PongBall> &ball,
                                     const curve::time_t &now);

}  // openage::event::demo
