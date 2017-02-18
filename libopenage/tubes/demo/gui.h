#pragma once

#include <vector>

#include "gamestate.h"

namespace openage {
namespace tubepong {


class Gui {

public:
	Gui();
	std::vector<event> &getInputs(const PongPlayer &player);
	void draw(PongState &state, const tube::tube_time_t &now);
	void draw_ball(util::vertex<2, float> ball, int idx);

private:
	std::vector<event> input_cache;
};

}} // openage::tubepong
