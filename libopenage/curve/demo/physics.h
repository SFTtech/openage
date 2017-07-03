// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "gamestate.h"
#include "../curve.h"
#include "../events.h"

namespace openage {
namespace curvepong {

class Physics {
public:

	void processInput(PongState &, PongPlayer &, std::vector<event> &input, curve::EventQueue *, const curve::curve_time_t &now);
protected:

	static void reset(PongState &, curve::EventQueue *, const curve::curve_time_t &);


	static void ball_reflect_wall(PongState &, curve::EventQueue *, const curve::curve_time_t &);
	static void ball_reflect_panel(PongState &, curve::EventQueue *, const curve::curve_time_t &);
	static void predict_reflect_wall(PongState &, curve::EventQueue *, const curve::curve_time_t &);
	static void predict_reflect_panel(PongState &, curve::EventQueue *, const curve::curve_time_t &);
};

}} // openage::curvepong
