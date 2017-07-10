// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "aicontroller.h"
#include "gamestate.h"
#include "gui.h"
#include "physics.h"

#include <chrono>

#include <unistd.h>

typedef std::chrono::high_resolution_clock Clock;

#define GUI
#define REALTIME 3
#undef HUMAN

namespace openage {
namespace curvepong {

int demo() {
	// Restart forever
#ifdef GUI
	curvepong::Gui gui;
#endif
	curve::EventQueue queue;
	curve::TriggerFactory factory{&queue};
	curvepong::Physics phys;
	curvepong::AIInput ai;
	bool running = true;
	srand(time(NULL));
	while (running) {
		curvepong::PongState state(&factory);
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
		now += 1;
		{
			std::vector<event> start_event { event(0, event::START) };
			phys.processInput(state, state.p1, start_event, &queue, now);
		}

#ifdef GUI
		gui.draw(state, now);  // initial drawing with corrected ball
#endif
		while (state.p1.lives.get(now) > 0 && state.p2.lives.get(now) > 0) {
			//We need to get the inputs to be able to kill the game
#ifdef HUMAN
			phys.processInput(state, state.p1, gui.getInputs(state.p1), &queue, now);
#else
			gui.getInputs(state.p1);
			phys.processInput(
				state, state.p1, ai.getInputs(state.p1, state.ball, now), &queue, now);
#endif
			phys.processInput(
				state, state.p2, ai.getInputs(state.p2, state.ball, now), &queue, now);

			state.p1.y = 0;
			state.p2.y = state.resolution[0] - 1;

			queue.execute_until(now + 100);
//			phys.update(state, now);
			queue.print();
#ifdef GUI
			gui.draw(state, now);
#endif
#if REALTIME == 1
			usleep(4000);
			double dt = std::chrono::duration_cast<std::chrono::milliseconds>(
			                (Clock::now() - loop_start))
			                .count();
			now += dt;
#elif REALTIME == 2
			now += 4;
#else
			now += 1;
			usleep(4000);
#endif
			loop_start = Clock::now();
		}
	}
	return 0;
}


}}  // openage::curvepong
