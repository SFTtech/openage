// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../continuous.h"
#include "../discrete.h"
#include "../../util/vector.h"

namespace openage {
namespace curvepong {

struct event {
	int player;
	enum state_e {
		UP, DOWN, START, IDLE, LOST
	} state;
	event(int id, state_e s) : player(id), state(s) {}
	event() : player(0), state(IDLE) {}
};

class PongPlayer {
public:
	PongPlayer() {
		speed.set_drop(0, 1);
		position.set_drop(0, 0.5);
		lives.set_drop(0, 1);
		state.set_drop(0, event(0, event::IDLE));
		size.set_drop(0, 0.1);
		y = 0;
		id = 0;
	}

	curve::Discrete<float> speed;
	curve::Continuous<float> position;
	curve::Discrete<int> lives;
	curve::Discrete<event> state;
	curve::Discrete<float> size;
	float y;
	int id;
};

class PongBall {
public:
	curve::Discrete<util::Vector<2>> speed;
	curve::Continuous<util::Vector<2>> position;
};

class PongState {
public:
	PongPlayer p1;
	PongPlayer p2;

	PongBall ball;

	util::Vector<2> resolution;
};

}} // openage::curvepong
