// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "gamestate.h"
#include "../tube.h"

namespace openage {
namespace tubepong {

class Physics {
public:
	void processInput(PongState &, PongPlayer &, std::vector<event> &events, const tube::tube_time_t &now);
	void update(PongState &, const tube::tube_time_t &);
protected:
	void update_ball(PongState &, const tube::tube_time_t &, int recursion_limit);
};

}} // openage::tubepong
