// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "gamestate.h"
#include "../curve.h"

namespace openage {
namespace curvepong {

class Physics {
public:
	void processInput(PongState &, PongPlayer &, std::vector<event> &events, const curve::curve_time_t &now);
	void update(PongState &, const curve::curve_time_t &);
protected:
	void update_ball(PongState &, const curve::curve_time_t &, int recursion_limit);
};

}} // openage::curvepong
