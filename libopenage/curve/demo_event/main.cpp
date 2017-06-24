// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <chrono>

#include <unistd.h>
#include <ncurses.h>

#include "aicontroller.h"
#include "gamestate.h"
#include "gui.h"
#include "physics.h"

typedef std::chrono::high_resolution_clock Clock;

#define GUI
#define REALTIME
#undef HUMAN

namespace openage {
namespace curvepong {

int demo() {
	// Restart forever
#ifdef GUI
	curvepong::Gui gui;
#endif
	curvepong::Physics phys;
	curvepong::AIInput ai;
	bool running = true;
	curve::EventQueue queue;
	srand(time(NULL));
	while (running) {
		curvepong::PongState state;
		curve::curve_time_t now = 1;

		state.p1.lives.set_drop(now, 3);
		state.p1.id = 0;
		state.p1.size.set_drop(now, 4);
		state.p2.lives.set_drop(now, 3);
		state.p2.id = 1;
		state.p2.size.set_drop(now, 4);
#ifdef GUI
		gui.draw(state, now);  // update gui related parameters
#else
		state.resolution[0] = 100;
		state.resolution[1] = 40;
#endif

		auto loop_start = Clock::now();
		curve::curve_time_t last_now = now;
		now += 1;
		{
			std::vector<event> start_event { event(0, event::START) };
			phys.processInput(state, state.p1, start_event, &queue, now);
		}

#ifdef GUI
		gui.draw(state, now);  // initial drawing with corrected ball
#endif
		while (state.p1.lives.get(now) > 0 && state.p2.lives.get(now) > 0) {
#ifdef HUMAN
			gui.getInputs(state.p1);
			phys.processInput(state, state.p1, gui.getInputs(state.p1), &queue, now);
#else
			phys.processInput(
				state, state.p1, ai.getInputs(state.p1, state.ball, now), &queue, now);
#endif
			phys.processInput(
				state, state.p2, ai.getInputs(state.p2, state.ball, now), &queue, now);

			state.p1.y = 0;
			state.p2.y = state.resolution[0] - 1;

			queue.execute_until(now + 10);
			last_now = now;
//			phys.update(state, now);
			queue.print();
#ifdef GUI
			gui.draw(state, now);
#endif
#ifdef REALTIME
			usleep(4000);
			double dt = std::chrono::duration_cast<std::chrono::milliseconds>(
			                (Clock::now() - loop_start))
			                .count();
			now += dt;
#else
			now += 4;
#endif
			loop_start = Clock::now();
		}
	}
	return 0;
}


}}  // openage::curvepong
