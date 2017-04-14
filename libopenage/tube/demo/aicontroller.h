// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "gamestate.h"

namespace openage {
namespace tubepong {

class AIInput {
public:
	std::vector<event> &getInputs(
		const PongPlayer &player,
		const PongBall &ball,
		const tube::tube_time_t &now);

private:
	std::vector<event> event_cache;
};

}} // openage::tubepong
