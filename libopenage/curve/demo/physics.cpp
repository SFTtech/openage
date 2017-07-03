// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "physics.h"
#include "../../error/error.h"

#include <cassert>
#include <cmath>

#include <ncurses.h>

#define GUI

namespace openage {
namespace curvepong {

const float extrapolating_time = 100.0f;

void Physics::processInput(PongState &state,
                           PongPlayer &player,
                           std::vector<event> &events,
                           curve::EventQueue *queue,
                           const curve::curve_time_t &now) {
	for (auto evnt : events) {
		//Process only if the future has changed
		if (player.state.get(now).state != evnt.state) {
			player.state.set_drop(now, evnt);

			switch(evnt.state) {
			case event::UP:
			case event::DOWN: {
				if (evnt.state == event::UP) {
					player.speed.set_drop(now, -2);
				} else if (evnt.state == event::DOWN) {
					player.speed.set_drop(now, 2);
				}
				player.speed.set_drop(now + extrapolating_time, 0);

				float new_pos = player.position.get(now) +
							(player.speed.get(now+extrapolating_time)
							 - player.speed.get(now) / 2 + player.speed.get(now));
				if (new_pos < 0)
					new_pos = 0;
				if (new_pos > state.resolution[1])
					new_pos = state.resolution[1];

				player.position.set_drop(now+extrapolating_time, new_pos);
				evnt.state = event::IDLE;
				player.state.set_drop(now + extrapolating_time, evnt);
//				predict_reflect_panel(state, queue, now);
				break;
			}
			case event::IDLE:
					player.position.set_drop(now+extrapolating_time,
						player.position.get(now));
				break;
			case event::START:
				reset(state, queue, now);
				break;

				//if (player.state.get(now).state == event::LOST) {
				//	state.ball.position.set_drop(now, state.resolution * 0.5);
				//}
				//update_ball(state, now, init_recursion_limit);
				//break;
			default:
				break;
			}
		}
	}
}


void Physics::reset(PongState &state,
                    curve::EventQueue *q,
                    const curve::curve_time_t &now) {
	q->clear(now);
	q->addcallback(now, "COLL WALL",
	                   [&state](curve::EventQueue *queue, const curve::curve_time_t &now) {
		                   state.ball.position.set_drop(now, state.resolution * 0.5);
		                   state.p1.position.set_drop(now, state.resolution[1] / 2);
		                   state.p2.position.set_drop(now, state.resolution[1] / 2);
		                   float dirx = (rand() % 2 * 2) - 1;
		                   float diry = (rand() % 2 * 2) - 1;
		                   auto init_speed =
		                   util::Vector<2>(
							   dirx * (0.0001 + (rand() % 100) / 1000.f),
							   diry * (0.0001 + (rand() % 100) / 2000.f));

						   state.ball.speed.set_drop(now, init_speed);
						   auto pos = state.ball.position.get(now);
		                   static int cnt = 0;
		                   mvprintw(20, 20, "Reset. Speed %f | %f POS %f | %f [%i]",
		                            init_speed[0],
		                            init_speed[1],
		                            pos[0],
		                            pos[1],
		                            ++cnt);
		                   predict_reflect_panel(state, queue, now);
		                   predict_reflect_wall(state, queue, now);
	                   });
}


void Physics::ball_reflect_wall(PongState &state,
                                curve::EventQueue *queue,
                                const curve::curve_time_t &now) {
	auto pos = state.ball.position.get(now);
	auto speed = state.ball.speed.get(now);
#ifndef GUI
	std::cout << "\nWall\n";
#else
	static int cnt = 0;
	mvprintw(22, 22, "Wall Reflect [%i]", ++cnt);
	mvprintw(23, 22, "Speed[1]        is %f", speed[1]);
#endif
	speed[1] *= -1.0;
	state.ball.speed.set_drop(now, speed);
	state.ball.position.set_drop(now, pos);

	predict_reflect_wall(state, queue, now);
}


void Physics::predict_reflect_wall(PongState &state,
                                   curve::EventQueue *queue,
                                   const curve::curve_time_t &now) {
	auto pos = state.ball.position.get(now);
	auto speed = state.ball.speed.get(now);

	double ty = 0;
	if (speed[1] > 0) {
		ty = (state.resolution[1] - pos[1]) / speed[1];
	} else if (speed[1] < 0) {
		ty = pos[1] / -speed[1];
	} else {
		throw Error(MSG(err) << "speed was 0");
	}
#ifdef GUI
	mvprintw(22, 40, "WALL TY %f NOW %f, NOWTY %f ", ty, now, now + ty);
#endif
	if (ty > 0) {
		auto hit_pos = pos + speed * ty;
		hit_pos[1] = speed[1] > 0 ? state.resolution[1] : 0;
		//assert(hit_pos[0] >= 0);
		assert(hit_pos[1] >= 0);
		state.ball.position.set_drop(now + ty, hit_pos);
		queue->addcallback(now + ty, "COLL WALL",
		    [&state](curve::EventQueue *q, const curve::curve_time_t &t) {
				ball_reflect_wall(state, q, t);
			});
	} else {
		throw Error(MSG(err) << "lost a callback because ty was " << ty << " <= 0");
	}
}


void Physics::ball_reflect_panel(PongState &state,
                                 curve::EventQueue *queue,
                                 const curve::curve_time_t &now) {

	auto pos = state.ball.position.get(now);
	auto speed = state.ball.speed.get(now);
#ifdef GUI
	static int cnt = 0;
	mvprintw(21, 22, "Panel hit [%i]     ", ++cnt);
#else
	std::cout << "\nPanel\n";
#endif

	if (pos[0] <= 1
	    && speed[0] < 0
	    && (pos[1] < state.p1.position.get(now) - state.p1.size.get(now) / 2
	        || pos[1] > state.p1.position.get(now) + state.p1.size.get(now) / 2)) {
		// Ball missed the paddel of player 1
		auto l = state.p1.lives(now);
		l --;
		state.p1.lives.set_drop(now, l);
		state.ball.position.set_drop(now, pos);
		reset(state, queue, now);
		mvprintw(21, 18, "1");
	} else if (pos[0] >= state.resolution[0]-1
	           && speed[0] > 0
	           && (pos[1] < state.p2.position.get(now) - state.p2.size.get(now) / 2
		        || pos[1] > state.p2.position.get(now) + state.p2.size.get(now) / 2)) {
		// Ball missed the paddel of player 2
		auto l = state.p2.lives(now);
		l --;
		state.p2.lives.set_drop(now, l);
		state.ball.position.set_drop(now, pos);
		reset(state, queue, now);
		mvprintw(21, 18, "2");
	} else if (pos[0] >= state.resolution[0]- 1 || pos[0] <= 1) {
		speed[0] *= -1.0;
		state.ball.speed.set_drop(now, speed);
		state.ball.position.set_drop(now, pos);
		predict_reflect_panel(state, queue, now);
	}
}


void Physics::predict_reflect_panel(PongState &state,
                                    curve::EventQueue *queue,
                                    const curve::curve_time_t &now) {
	auto pos = state.ball.position.get(now);
	auto speed = state.ball.speed.get(now);
//	speed[1] += (rand() % 50 - 50)/10;
	float ty = 0;
	if (speed[0] > 0) {
		ty = (state.resolution[0] - pos[0]) / speed[0];
	} else if (speed[0] < 0) {
		ty = pos[0] / -speed[0];
	}
#ifdef GUI
	mvprintw(21, 40, "AT %f NEXT %f", now, now + ty);
	mvprintw(21, 18, "2");
#endif
	auto hit_pos = pos + speed * ty;
	hit_pos[0] = speed[0] > 0 ? state.resolution[0] : 0;
	assert(hit_pos[0] >= 0);
//	assert(hit_pos[1] >= 0); // this might validly happen
	state.ball.position.set_drop(now + ty, hit_pos);
	queue->addcallback(now + ty, "COLL PANEL",
	    [&state](curve::EventQueue *q, const curve::curve_time_t &t) {
			ball_reflect_panel(state, q, t);
		});

//	predict_reflect_wall(state, queue, now);
}

}} // openage::curvepong
