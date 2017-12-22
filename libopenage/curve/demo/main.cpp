// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "config.h"
#include "aicontroller.h"
#include "gamestate.h"
#include "gui.h"
#include "physics.h"

#include "../events/event.h"

#include <chrono>

#include <unistd.h>

#include <ncurses.h>

typedef std::chrono::high_resolution_clock Clock;

namespace openage {
namespace curvepong {

int demo() {
	// Restart forever
#ifdef GUI
	curvepong::Gui gui;
#endif
bool running = true;
	srand(time(NULL));
	while (running) {
		curve::EventManager events;
		curve::curve_time_t now = 1;
		curvepong::Physics phys;
		curvepong::AIInput ai;
		auto state = std::make_shared<State>(&events);
		curvepong::Physics::init(state, &events, now);

		state->p1->lives->set_drop(now, 3);
		state->p1->size->set_drop(now, 4);

		state->p2->lives->set_drop(now, 3);
		state->p2->size->set_drop(now, 4);
#ifdef GUI
		gui.draw(state, now);  // update gui related parameters
#else
		state->resolution[0] = 100;
		state->resolution[1] = 40;
#endif

		auto loop_start = Clock::now();
		now += 1;
		{
			std::vector<event> start_event { event(0, event::START) };
			phys.processInput(state, state->p1, start_event, &events, now);
		}

#ifdef GUI
		gui.draw(state, now);  // initial drawing with corrected ball
#endif
		while (state->p1->lives->get(now) > 0 && state->p2->lives->get(now) > 0) {
			//We need to get the inputs to be able to kill the game
#ifdef HUMAN
			phys.processInput(state, state->p1, gui.getInputs(state->p1), &events, now);
#else
#ifdef GUI
			gui.getInputs(state->p1);
#endif

			phys.processInput(
				state, state->p1, ai.getInputs(state->p1, state->ball, now), &events, now);
#endif
			phys.processInput(
				state, state->p2, ai.getInputs(state->p2, state->ball, now), &events, now);

			state->p1->y = 0;
			state->p2->y = state->resolution[0] - 1;

			events.execute_until(now, state);
//			phys.update(state, now);
#ifdef GUI
			gui.draw(state, now);


			int pos = 1;
			mvprintw(pos++, state->resolution[0]/2 + 10, "Queue: ");
			for (const auto & e : events.queue.ro_queue()) {
				mvprintw(pos++, state->resolution[0]/2 + 10,
				         "%d: %s ",
				         e->time(), e->eventclass()->id().c_str());
			}

#endif

#if REALTIME == 0
			double dt = std::chrono::duration_cast<std::chrono::milliseconds>(
				(Clock::now() - loop_start))
			            .count();
			if (dt < 12000) {
				usleep(12000 - dt);
			}
			now += dt;
#elif REALTIME == 1
			now += 1;
			usleep(20000);
#elif REALTIME == 2
			now += 4;
#else
			#error no REALTIME plan set
#endif
			loop_start = Clock::now();
		}
	}
	return 0;
}


}}  // openage::curvepong
