// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "gamestate.h"

namespace openage {
namespace curvepong {


class Gui {

public:
	Gui();
	std::vector<event> &getInputs(const PongPlayer &player);
	void draw(PongState &state, const curve::curve_time_t &now);
	void draw_ball(util::Vector<2> ball, char chr);

private:
	std::vector<event> input_cache;
};

}} // openage::curvepong
