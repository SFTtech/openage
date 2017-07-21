// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "physics.h"

#include "../../error/error.h"

#include <cassert>
#include <cmath>

#include <ncurses.h>

namespace openage {
namespace curvepong {

const float extrapolating_time = 100.0f;

using namespace curve;

class BallReflectWall : public EventClass {
public:
	BallReflectWall () : EventClass("demo.ball.reflect_wall", EventClass::Type::ON_CHANGE) {} // FIXME: on_change as own class

	void setup(const std::shared_ptr<Event> &evnt,
	           const std::shared_ptr<State> &state) override {
		this->add_dependency(evnt, state->ball->position); // FIXME: add_trigger (and warn if not on_change)
		this->add_dependency(evnt, state->ball->speed);
		// TODO add dependency to size of area
		// FIXME dependency to a full ball object
	}

	// FIXME we REALLY need dependencies to objects i.e. Ball : public EventTarget()
	// FIXME rename to invoke
	// FIXME EventTarget for BALL!!
	void call(curve::EventManager *,
	          const std::shared_ptr<EventTarget> &target,
	          const std::shared_ptr<State> &state,
	          const curve::curve_time_t &now,
              const EventClass::param_map &/*param*/) override {
		// TODO Do we really need to check for the time?
		auto positioncurve = std::dynamic_pointer_cast<Continuous<util::Vector<2>>>(target);
		auto speedcurve = state->ball->speed;

		// All the magic in the next lines
		auto speed = speedcurve->get(now);
		auto pos = positioncurve->get(now);
		speed[1] *= -1.0;
		state->ball->speed->set_drop(now, speed);
		state->ball->position->set_drop(now, pos);

		if (speed[1] == 0)
			return;

		double ty = 0;
		if (speed[1] > 0) {
			// fixme rename resolution to display_boundary
			ty = (state->resolution[1] - pos[1]) / speed[1];
		} else if (speed[1] < 0) {
			ty = pos[1] / -speed[1];
		} else {
		}
		state->ball->position->set_drop(now+ty, pos + speed * ty);
	}

	// FIXME rename to predict_invoke_time
	curve_time_t recalculate_time(const std::shared_ptr<EventTarget> &target,
	                              const std::shared_ptr<State> &state,
	                              const curve_time_t &now) override {
		auto positioncurve = std::dynamic_pointer_cast<Continuous<util::Vector<2>>>(target);
		auto speed = state->ball->speed->get(now);
		auto pos = positioncurve->get(now);

		if (speed[1] == 0) {
			return std::numeric_limits<curve::curve_time_t>::max();
		}
		double ty = 0;
		if (speed[1] > 0) {
			ty = (state->resolution[1] - pos[1]) / speed[1];
		} else if (speed[1] < 0) {
			ty = pos[1] / -speed[1];
		} else {
		}
#ifdef GUI
		mvprintw(22, 40, "WALL TY %f NOW %f, NOWTY %f ", ty, now, now + ty);
#endif
		return now + ty;
	}
};

class BallReflectPanel : public curve::EventClass {
public:
	BallReflectPanel () : curve::EventClass("demo.ball.reflect_panel", EventClass::Type::ON_CHANGE) {}

	void setup(const std::shared_ptr<Event> &target,
	           const std::shared_ptr<State> &state) override {
		add_dependency(target, state->ball->position);
		add_dependency(target, state->ball->speed);
		add_dependency(target, state->p1->position);
		add_dependency(target, state->p2->position);
		// TODO add dependency to size of area
		// FIXME dependency to a full ball object
	}

	// FIXME we REALLY need dependencies to objects
	void call(curve::EventManager *mgr,
	          const std::shared_ptr<EventTarget> & /*target*/,
	          const std::shared_ptr<State> &state,
	          const curve::curve_time_t &now,
              const EventClass::param_map &/*param*/) override {

		auto pos = state->ball->position->get(now);
		auto speed = state->ball->speed->get(now);
#ifdef GUI
		static int cnt = 0;
		mvprintw(21, 22, "Panel hit [%i]     ", ++cnt);
#else
		std::cout << "\nPanel\n";
#endif

		if (pos[0] <= 1
		    && speed[0] < 0
		    && (pos[1] < state->p1->position->get(now) - state->p1->size->get(now) / 2
		        || pos[1] > state->p1->position->get(now) + state->p1->size->get(now) / 2)) {
			// Ball missed the paddel of player 1
			auto l = state->p1->lives->get(now);
			l--;
			state->
			p1->lives->set_drop(now, l);
			state->ball->position->set_drop(now, pos);
			Physics::reset(state, mgr, now);
			mvprintw(21, 18, "1");
		} else if (pos[0] >= state->resolution[0]-1
		           && speed[0] > 0
		           && (pos[1] < state->p2->position->get(now) - state->p2->size->get(now) / 2
		               || pos[1] > state->p2->position->get(now) + state->p2->size->get(now) / 2)) {
			// Ball missed the paddel of player 2
			auto l = state->p2->lives->get(now);
			l --;
			state->p2->lives->set_drop(now, l);
			state->ball->position->set_drop(now, pos);
			Physics::reset(state, mgr, now);
			mvprintw(21, 18, "2");
		} else if (pos[0] >= state->resolution[0]- 1 || pos[0] <= 1) {
			speed[0] *= -1;
			state->ball->speed->set_drop(now, speed);
			state->ball->position->set_drop(now, pos);
		}

		float ty = 0;
		if (speed[0] > 0) {
			ty = (state->resolution[0] - pos[0]) / speed[0];
		} else if (speed[0] < 0) {
			ty = pos[0] / -speed[0];
		}
		auto hit_pos = pos + speed * ty;
		hit_pos[0] = speed[0] > 0 ? state->resolution[0] : 0;
		state->ball->position->set_drop(now+ty, hit_pos);
	}

	curve_time_t recalculate_time(const std::shared_ptr<EventTarget> &target,
	                              const std::shared_ptr<State> &state,
	                              const curve_time_t &now) override {
		auto positioncurve = std::dynamic_pointer_cast<Continuous<util::Vector<2>>>(target);
		auto speed = state->ball->speed->get(now);
		auto pos = positioncurve->get(now);

		if (speed[0] == 0)
			return std::numeric_limits<curve::curve_time_t>::max();
		float ty = 0;
		if (speed[0] > 0) {
			ty = (state->resolution[0] - pos[0]) / speed[0];
		} else if (speed[0] < 0) {
			ty = pos[0] / -speed[0];
		}
#ifdef GUI
		mvprintw(21, 40, "AT %f NEXT %f", now, now + ty);
		mvprintw(21, 18, "2");
#endif
		auto hit_pos = pos + speed * ty;
		hit_pos[0] = speed[0] > 0 ? state->resolution[0] : 0;
		assert(hit_pos[0] >= 0);
		return now + ty;
	}
};


class ResetGame : public curve::EventClass {
public:
	ResetGame () : curve::EventClass("demo.reset", EventClass::Type::ONCE) {}

	void setup(const std::shared_ptr<Event> &target, const std::shared_ptr<State> &state) override {
		(void)target;
		(void)state;
	}

	void call(curve::EventManager *mgr,
	          const std::shared_ptr<EventTarget> &target,
	          const std::shared_ptr<State> &state,
	          const curve::curve_time_t &now,
              const EventClass::param_map &/*param*/) override {
		(void) mgr;
		(void) target;

		// Check if the condition still applies
		{
			auto pos = state->ball->position->get(now);
			if (pos[0] > 0 && pos[0] < state->resolution[0]) {
				// the gamestate is still valid - there is no need to reset
				return;
			}
		}

		state->ball->position->set_drop(now-0.1, state->ball->position->get(now));
		state->ball->position->set_drop(now, state->resolution * 0.5);
		state->p1->position->set_drop(now, state->resolution[1] / 2);
		state->p2->position->set_drop(now, state->resolution[1] / 2);

		float dirx = 1;//(rand() % 2 * 2) - 1;
		float diry = 0.1;//(rand() % 2 * 2) - 1;
		auto init_speed =
		util::Vector<2>(
			dirx * (0.001 + (rand() % 100) / 100.f),
			diry * (0.001 + (rand() % 100) / 200.f));

		state->ball->speed->set_drop(now, init_speed);
		auto pos = state->ball->position->get(now);
		static int cnt = 0;
		mvprintw(20, 20, "Reset. Speed %f | %f POS %f | %f [%i]",
		         init_speed[0],
		         init_speed[1],
		         pos[0],
		         pos[1],
		         ++cnt);

		double ty = 0;
		if (init_speed[1] > 0) {
			ty = (state->resolution[1] - pos[1]) / init_speed[1];
		} else if (init_speed[1] < 0) {
			ty = pos[1] / -init_speed[1];
		} else {
		}
		state->ball->position->set_drop(now+ty, pos + init_speed * ty);
	}

	curve_time_t recalculate_time(const std::shared_ptr<EventTarget> &target,
	                              const std::shared_ptr<State> &state,
	                              const curve_time_t &old_time) override {
		(void) target;
		(void) state;
		return old_time;
	}
};

void Physics::init(std::shared_ptr<State> &state,
                   curve::EventManager *mgr,
                   const curve::curve_time_t &now) {
#ifndef GUI
	std::cout << "Init" << std::endl;
#endif
	mgr->add_class(std::make_shared<BallReflectPanel>());
	mgr->add_class(std::make_shared<BallReflectWall>());
	mgr->add_class(std::make_shared<ResetGame>());

	mgr->on("demo.ball.reflect_wall", state->ball->position, state, now);
	mgr->on("demo.ball.reflect_panel", state->ball->position, state, now);
	// FIXME once "reset": deregister

//	reset(state, mgr, now);
}

void Physics::processInput(std::shared_ptr<State> &state,
                           std::shared_ptr<PongPlayer> &player,
                           std::vector<event> &events,
                           EventManager *mgr,
                           const curve_time_t &now) {
	for (auto evnt : events) {
		//Process only if the future has changed
		if (player->state->get(now).state != evnt.state) {
			player->state->set_drop(now, evnt);

			switch(evnt.state) {
			case event::UP:
			case event::DOWN: {
				if (evnt.state == event::UP) {
					player->speed->set_drop(now, -2);
				} else if (evnt.state == event::DOWN) {
					player->speed->set_drop(now, 2);
				}
				player->speed->set_drop(now + extrapolating_time, 0);

				float new_pos = player->position->get(now) +
							(player->speed->get(now+extrapolating_time)
							 - player->speed->get(now) / 2 + player->speed->get(now));
				if (new_pos < 0)
					new_pos = 0;
				if (new_pos > state->resolution[1])
					new_pos = state->resolution[1];

				player->position->set_drop(now+extrapolating_time, new_pos);
				evnt.state = event::IDLE;
				player->state->set_drop(now + extrapolating_time, evnt);
//				predict_reflect_panel(state, queue, now);
				break;
			}
			case event::IDLE:
//					player->position->set_drop(now+extrapolating_time,
//						player->position->get(now));
				break;
			case event::START:
				reset(state, mgr, now);
				break;

				//if (player->state->get(now).state == event::LOST) {
				//	state->ball.position->set_drop(now, state.resolution * 0.5);
				//}
				//update_ball(state, now, init_recursion_limit);
				//break;
			default:
				break;
			}
		}
	}
}


void Physics::reset(const std::shared_ptr<State> &state,
                    curve::EventManager *mgr,
                    const curve::curve_time_t &now) {
	mgr->on("demo.reset", state->ball->position, state, now);
}

}} // openage::curvepong
