// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "config.h"
#include "gamestate.h"

#include <vector>

namespace openage {
namespace curvepong {


class Gui {

public:
	Gui();
	std::vector<event> &getInputs(const std::shared_ptr<PongPlayer> &player);
	void draw(std::shared_ptr<State> &state, const curve::curve_time_t &now);
	void draw_ball(util::Vector<2> ball, char chr);

private:
	std::vector<event> input_cache;
};

}} // openage::curvepong
