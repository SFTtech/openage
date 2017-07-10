// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate.h"

#include <vector>

namespace openage {
namespace curvepong {

class AIInput {
public:
	std::vector<event> &getInputs(
		const PongPlayer &player,
		const PongBall &ball,
		const curve::curve_time_t &now);

private:
	std::vector<event> event_cache;
};

}} // openage::curvepong
