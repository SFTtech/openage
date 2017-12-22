// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "config.h"

#include "../events/eventtarget.h"
#include "../continuous.h"
#include "../discrete.h"
#include "../../util/vector.h"

#include <functional>
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


using namespace std::placeholders;
class PongPlayer : public openage::curve::EventTarget {
public:
	PongPlayer(curve::EventManager *mgr, size_t id) :
		EventTarget(mgr),
		speed(std::make_shared<curve::Discrete<float>>(
			      mgr,
			      (id << 4) + 1,
			      std::bind(&PongPlayer::child_changed, this, _1))),
		position(std::make_shared<curve::Continuous<float>>(
			         mgr,
			         (id << 4) + 2,
			         std::bind(&PongPlayer::child_changed, this, _1))),
		lives(std::make_shared<curve::Discrete<int>>(
			      mgr,
			      (id << 4) + 3,
			      std::bind(&PongPlayer::child_changed, this, _1))),
		state(std::make_shared<curve::Discrete<event>>(
			      mgr,
			      (id << 4) + 4,
			      std::bind(&PongPlayer::child_changed, this, _1))),
		size(std::make_shared<curve::Discrete<float>>(
			     mgr,
			     (id << 4) + 5,
			     std::bind(&PongPlayer::child_changed, this, _1))),
		_id{id},
		y{0} {}

	std::shared_ptr<curve::Discrete<float>> speed;
	std::shared_ptr<curve::Continuous<float>> position;
	std::shared_ptr<curve::Discrete<int>> lives;
	std::shared_ptr<curve::Discrete<event>> state;
	std::shared_ptr<curve::Discrete<float>> size;
	size_t _id;
	float y;

	size_t id() const override{
		return _id;
	}
private:
	void child_changed(const curve::curve_time_t &time) {
		this->on_change(time);
	}
};


class PongBall : public openage::curve::EventTarget {
public:
	PongBall(curve::EventManager *mgr,size_t id) :
		EventTarget(mgr),
		speed(std::make_shared<curve::Discrete<util::Vector<2>>>(
			      mgr,
			      (id << 2) + 1,
			      std::bind(&PongBall::child_changed, this, _1))),
		position(std::make_shared<curve::Continuous<util::Vector<2>>>(
			         mgr,
			         (id << 2) + 2,
			         std::bind(&PongBall::child_changed, this, _1))),
		_id{id}
		{}

	std::shared_ptr<curve::Discrete<util::Vector<2>>> speed;
	std::shared_ptr<curve::Continuous<util::Vector<2>>> position;

	size_t id() const override{
		return _id;
	}
private:
	void child_changed(const curve::curve_time_t &time) {
		this->on_change(time);
	}
	size_t _id;
};
}} // namespace openage::curvepong


namespace openage {
class State {
public:
	State(curve::EventManager *mgr) :
		p1(std::make_shared<curvepong::PongPlayer>(mgr, 0)),
		p2(std::make_shared<curvepong::PongPlayer>(mgr, 1)),
		ball(std::make_shared<curvepong::PongBall>(mgr, 2)) {}

	std::shared_ptr<curvepong::PongPlayer> p1;
	std::shared_ptr<curvepong::PongPlayer> p2;
	std::shared_ptr<curvepong::PongBall> ball;
	util::Vector<2> resolution;
};

} // namespace openage
