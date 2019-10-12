// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

#include <nyan/nyan.h>

#include "config.h"
#include "../../curve/continuous.h"
#include "../../curve/discrete.h"
#include "../../curve/segmented.h"
#include "../../event/evententity.h"
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


class PongPlayer : public event::EventEntity {
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

private:
	void child_changes(const curve::time_t &time);
};


class PongBall : public event::EventEntity {
public:
	PongBall(const std::shared_ptr<event::Loop> &mgr, size_t id);

	size_t id() const override;
	std::string idstr() const override;

	std::shared_ptr<curve::Discrete<util::Vector2d>> speed;
	std::shared_ptr<curve::Segmented<util::Vector2d>> position;

private:
	void child_changes(const curve::time_t &time);
	size_t _id;
};


class PongState : public event::State {
public:
	PongState(const std::shared_ptr<event::Loop> &mgr,
	          const std::shared_ptr<Gui> &gui,
	          const std::shared_ptr<nyan::View> &dbroot);

	std::shared_ptr<PongPlayer> p1;
	std::shared_ptr<PongPlayer> p2;
	std::shared_ptr<PongBall> ball;
	std::shared_ptr<curve::Discrete<util::Vector2s>> area_size;

	std::shared_ptr<Gui> gui;

	std::shared_ptr<nyan::View> dbroot;
	nyan::Object ballcfg;
};


} // openage::main::tests::pong
