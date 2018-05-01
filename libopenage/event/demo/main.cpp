// Copyright 2016-2018 the openage authors. See copying.md for legal info.

#include <chrono>
#include <ratio>
#include <unistd.h>

#include "../../config.h"
#include "../event.h"
#include "aicontroller.h"
#include "gamestate.h"
#include "gui.h"
#include "physics.h"

#if WITH_NCURSES
#include <ncurses.h>
#endif


using Clock = std::chrono::high_resolution_clock;
using namespace std::literals::chrono_literals;


namespace openage::event::demo {

/**
 * Simulation speeds
 */
enum class timescale {
	NOSLEEP,
	REALTIME,
	SLOW,
	FAST,
};


void curvepong(bool disable_gui, bool no_human) {

	bool enable_gui = not disable_gui;
	bool human_player = not no_human;

	timescale speed = timescale::REALTIME;


#if WITH_NCURSES
	Gui gui;
	if (enable_gui) {
		gui.init();
	}
#endif

	bool running = true;

	while (running) {
		auto loop = std::make_shared<Loop>();
		curve::time_t now = 1;
		Physics phys;
		AIInput ai;

		auto state = std::make_shared<PongState>(loop, enable_gui);
		Physics::init(state, loop, now);

		state->p1->lives->set_last(now, 3);
		state->p1->size->set_last(now, 4);

		state->p2->lives->set_last(now, 3);
		state->p2->size->set_last(now, 4);

#if WITH_NCURSES
		if (state->enable_gui) {
			gui.clear();
			gui.draw(state, now);  // update gui related parameters
		}
		else {
#endif
			state->display_boundary = {100, 40};
#if WITH_NCURSES
		}
#endif

		{
			std::vector<PongEvent> start_event{PongEvent(0, PongEvent::START)};
			phys.process_input(state, state->p1, start_event, loop, now);
		}

#if WITH_NCURSES
		if (state->enable_gui) {
			gui.draw(state, now);  // initial drawing with corrected ball
		}
#endif

		// this is the game loop, running while both players live!
		while (state->p1->lives->get(now) > 0 and
		       state->p2->lives->get(now) > 0) {

			auto loop_start = Clock::now();

#if WITH_NCURSES
			if (enable_gui) {
				gui.clear();
			}
#endif

			// process the input for both players
			// player 1 can be AI or human.

			if (human_player) {
#if WITH_NCURSES
				auto inputs = gui.get_inputs(state->p1);
#else
				// TODO: take terminal input without ncurses, if on tty?
				//       e.g. 'q' to close it?
				std::vector<PongEvent> inputs;
#endif
				phys.process_input(state, state->p1,
				                   inputs, loop, now);
			}
			else {
#if WITH_NCURSES
				// We still need to get the inputs to be able to kill the game
				if (state->enable_gui) {
					gui.get_inputs(state->p1);
				}
#endif

				phys.process_input(state, state->p1,
				                   ai.get_inputs(state->p1, state->ball, now),
				                   loop, now);
			}

			phys.process_input(state, state->p2,
			                   ai.get_inputs(state->p2, state->ball, now),
			                   loop, now);

			// paddle x positions
			state->p1->paddle_x = 0;
			state->p2->paddle_x = state->display_boundary[0] - 1;

			// run the event queue!
			loop->reach_time(now, state);

#if WITH_NCURSES
			if (state->enable_gui) {
				gui.draw(state, now);

				int pos = 1;
				mvprintw(pos++, state->display_boundary[0]/2 + 10, "Enqueued events:");
				for (const auto &e : loop->get_queue().get_event_queue()) {
					mvprintw(pos++, state->display_boundary[0]/2 + 10,
					         "%f: %s",
					         e->get_time().to_double(),
					         e->get_eventclass()->id().c_str());
				}

				gui.update_screen();
			}
#endif

			// handle timing for screen refresh and simulation advancement

			using dt_s_t = std::chrono::duration<double, std::ratio<1>>;
			using dt_us_t = std::chrono::duration<double, std::micro>;

			// microseconds per frame
			// 30fps = 1s/30 = 1000000us/30 per frame
			constexpr static std::chrono::microseconds per_frame = 33333us;
			constexpr static curve::time_t per_frame_s = std::chrono::duration_cast<dt_s_t>(per_frame).count();

			if (speed == timescale::NOSLEEP) {
				// increase the simulation loop time a bit
				usleep(5000);
			}

			dt_us_t dt_us = Clock::now() - loop_start;

			if (speed != timescale::NOSLEEP) {
				dt_us_t wait_time = per_frame - dt_us;

				if (wait_time > dt_us_t::zero()) {
					usleep(wait_time.count());
				}
			}

			switch (speed) {
			case timescale::NOSLEEP:
				// advance the frame calculation time only,
				// because we didn't sleep
				now += std::chrono::duration_cast<dt_s_t>(dt_us).count();
				break;

			case timescale::REALTIME:
				// advance the game time in seconds
				now += per_frame_s;
				break;

			case timescale::SLOW:
				now += per_frame_s / 10;
				break;

			case timescale::FAST:
				now += per_frame_s * 4;
				break;
			}
		}
	}
}


} // openage::event::demo
