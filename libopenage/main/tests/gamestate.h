// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

#include "config.h"
#include "../../curve/continuous.h"
#include "../../curve/discrete.h"
#include "../../event/eventtarget.h"
#include "../../event/loop.h"
#include "../../event/state.h"
#include "../../util/vector.h"


namespace openage::main::tests::pong {

class Gui;


class PongEvent {
public:
	enum state_e {
		UP, DOWN, START, IDLE, LOST
	};

	PongEvent(size_t id, state_e s) : player(id), state(s) {}
	PongEvent() : player(0), state(IDLE) {}

	size_t player;
	state_e state;
};


class PongPlayer : public event::EventTarget {
public:
	PongPlayer(const std::shared_ptr<event::Loop> &mgr, size_t id);

	size_t id() const override;
	std::string idstr() const override;

	std::shared_ptr<curve::Discrete<float>> speed;
	std::shared_ptr<curve::Continuous<float>> position;
	std::shared_ptr<curve::Discrete<int>> lives;
	std::shared_ptr<curve::Discrete<PongEvent>> state;
	std::shared_ptr<curve::Discrete<float>> size;

	size_t _id;
	float paddle_x;

private:
	void child_changes(const curve::time_t &time);
};


class PongBall : public event::EventTarget {
public:
	PongBall(const std::shared_ptr<event::Loop> &mgr, size_t id);

	size_t id() const override;
	std::string idstr() const override;

	std::shared_ptr<curve::Discrete<util::Vector2d>> speed;
	std::shared_ptr<curve::Continuous<util::Vector2d>> position;

private:
	void child_changes(const curve::time_t &time);
	size_t _id;
};


class PongState : public event::State {
public:
	PongState(const std::shared_ptr<event::Loop> &mgr,
	          const std::shared_ptr<Gui> &gui);

	std::shared_ptr<PongPlayer> p1;
	std::shared_ptr<PongPlayer> p2;
	std::shared_ptr<PongBall> ball;
	util::Vector2d display_boundary;

	std::shared_ptr<Gui> gui;
};


} // openage::main::tests::pong
