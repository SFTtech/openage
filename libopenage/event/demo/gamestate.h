// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <sstream>

#include "config.h"
#include "curve/continuous.h"
#include "curve/discrete.h"
#include "event/event_loop.h"
#include "event/evententity.h"
#include "event/state.h"
#include "util/strings.h"
#include "util/vector.h"


namespace openage::event::demo {

#if WITH_NCURSES
class Gui;
#endif


class PongEvent {
public:
	enum state_e {
		UP,
		DOWN,
		START,
		IDLE,
		LOST
	};

	PongEvent(size_t id, state_e s) :
		player(id), state(s) {}
	PongEvent() :
		player(0), state(IDLE) {}

	size_t player;
	state_e state;
};


using namespace std::placeholders;
class PongPlayer : public EventEntity {
public:
	PongPlayer(const std::shared_ptr<EventLoop> &mgr, size_t id) :
		EventEntity{mgr},
		speed(std::make_shared<curve::Discrete<float>>(
			mgr,
			(id << 4) + 1,
			util::sformat("PongPlayer(%zd).speed", id),
			std::bind(&PongPlayer::child_changes, this, _1))),
		position(std::make_shared<curve::Continuous<float>>(
			mgr,
			(id << 4) + 2,
			util::sformat("PongPlayer(%zd).position", id),
			std::bind(&PongPlayer::child_changes, this, _1))),
		lives(std::make_shared<curve::Discrete<int>>(
			mgr,
			(id << 4) + 3,
			util::sformat("PongPlayer(%zd).lives", id),
			std::bind(&PongPlayer::child_changes, this, _1))),
		state(std::make_shared<curve::Discrete<PongEvent>>(
			mgr,
			(id << 4) + 4,
			util::sformat("PongPlayer(%zd).state", id),
			std::bind(&PongPlayer::child_changes, this, _1))),
		size(std::make_shared<curve::Discrete<float>>(
			mgr,
			(id << 4) + 5,
			util::sformat("PongPlayer(%zd).size", id),
			std::bind(&PongPlayer::child_changes, this, _1))),
		_id{id},
		paddle_x{0} {}

	std::shared_ptr<curve::Discrete<float>> speed;
	std::shared_ptr<curve::Continuous<float>> position;
	std::shared_ptr<curve::Discrete<int>> lives;
	std::shared_ptr<curve::Discrete<PongEvent>> state;
	std::shared_ptr<curve::Discrete<float>> size;

	size_t _id;
	float paddle_x;

	size_t id() const override {
		return _id;
	}

	std::string idstr() const override {
		std::stringstream ss;
		ss << "PongPlayer[" << this->id() << "]";
		return ss.str();
	}

private:
	void child_changes(const curve::time_t &time) {
		this->changes(time);
	}
};


class PongBall : public EventEntity {
public:
	PongBall(const std::shared_ptr<EventLoop> &mgr, size_t id) :
		EventEntity{mgr},
		speed(std::make_shared<curve::Discrete<util::Vector2d>>(
			mgr,
			(id << 2) + 1,
			util::sformat("PongBall(%zd).speed", id),
			std::bind(&PongBall::child_changes, this, _1))),
		position(std::make_shared<curve::Continuous<util::Vector2d>>(
			mgr,
			(id << 2) + 2,
			util::sformat("PongBall(%zd).position", id),
			std::bind(&PongBall::child_changes, this, _1))),
		_id{id} {}

	std::shared_ptr<curve::Discrete<util::Vector2d>> speed;
	std::shared_ptr<curve::Continuous<util::Vector2d>> position;

	size_t id() const override {
		return _id;
	}

	std::string idstr() const override {
		std::stringstream ss;
		ss << "PongBall[" << this->id() << "]";
		return ss.str();
	}

private:
	void child_changes(const curve::time_t &time) {
		this->changes(time);
	}
	size_t _id;
};


class PongState : public State {
public:
	PongState(const std::shared_ptr<EventLoop> &mgr, bool enable_gui
#if WITH_NCURSES
	          ,
	          const std::shared_ptr<Gui> &gui
#endif
	          ) :
		State{mgr},
		p1(std::make_shared<PongPlayer>(mgr, 0)),
		p2(std::make_shared<PongPlayer>(mgr, 1)),
		ball(std::make_shared<PongBall>(mgr, 2)),
		enable_gui{enable_gui}
#if WITH_NCURSES
		,
		gui{gui}
#endif
	{
	}

	std::shared_ptr<PongPlayer> p1;
	std::shared_ptr<PongPlayer> p2;
	std::shared_ptr<PongBall> ball;
	util::Vector2d display_boundary;

	bool enable_gui;

#if WITH_NCURSES
	std::shared_ptr<Gui> gui;
#endif
};

} // namespace openage::event::demo
