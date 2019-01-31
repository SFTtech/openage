// Copyright 2016-2019 the openage authors. See copying.md for legal info.

#include "main.h"

#include <chrono>
#include <ratio>
#include <SDL.h>

#include "config.h"
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
	std::shared_ptr<Gui> gui = std::make_shared<Gui>();

	if (enable_gui) {
		gui->init();
	}
#endif

	bool running = true;

	while (running) {
		auto loop = std::make_shared<Loop>();
		curve::time_t now = 1;
		Physics phys;
		AIInput ai;

		auto state = std::make_shared<PongState>(loop, enable_gui
#if WITH_NCURSES
		                                         , gui
#endif
		);
		Physics::init(state, loop, now);

		state->p1->lives->set_last(now, 3);
		state->p1->size->set_last(now, 4);

		state->p2->lives->set_last(now, 3);
		state->p2->size->set_last(now, 4);

#if WITH_NCURSES
		if (state->enable_gui) {
			gui->clear();
			gui->get_display_size(state, now);  // update gui related parameters
		}
		else {
#endif
			state->display_boundary = {100, 40};
#if WITH_NCURSES
		}
#endif

		// initialize the game by resetting physics
		std::vector<PongEvent> start_event{PongEvent{0, PongEvent::START}};
		phys.process_input(state, state->p1, start_event, loop, now);


		std::vector<PongEvent> inputs;

		// this is the game loop, running while both players live!
		while (state->p1->lives->get(now) > 0 and
		       state->p2->lives->get(now) > 0) {

			auto loop_start = Clock::now();

#if WITH_NCURSES
			if (enable_gui) {
				gui->clear();
				gui->get_display_size(state, now);
			}
#endif

			// process the input for both players
			// player 1 can be AI or human.

			if (human_player) {
				phys.process_input(state, state->p1,
				                   inputs, loop, now);
			}
			else {

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

			// evaluate the event queue to reach the desired game time!
			loop->reach_time(now, state);

#if WITH_NCURSES
			if (state->enable_gui) {
				gui->draw(state, now);

				int pos = 1;
				mvprintw(pos++, state->display_boundary[0]/2 + 10, "Enqueued events:");
				for (const auto &e : loop->get_queue().get_event_queue().get_sorted_events()) {
					mvprintw(pos++, state->display_boundary[0]/2 + 10,
					         "%f: %s",
					         e->get_time().to_double(),
					         e->get_eventclass()->id().c_str());
				}

				gui->update_screen();

				// get the input after the current frame, because the get_inputs
				// implicitly updates the screen. if this is done too early,
				// the screen will be updated to be empty -> flickering.
				// TODO: take terminal input without ncurses, if on tty?
				//       e.g. 'q' to close it?
				inputs = gui->get_inputs(state->p1);
			}
#endif

			// handle timing for screen refresh and simulation advancement
			using dt_s_t = std::chrono::duration<double, std::ratio<1>>;
			using dt_ms_t = std::chrono::duration<double, std::milli>;

			// microseconds per frame
			// 30fps = 1s/30 = 1000000us/30 per frame
			constexpr static std::chrono::microseconds per_frame = 33333us;
			constexpr static curve::time_t per_frame_s = std::chrono::duration_cast<dt_s_t>(per_frame).count();

			if (speed == timescale::NOSLEEP) {
				// increase the simulation loop time a bit
				SDL_Delay(5);
			}

			dt_ms_t dt_us = Clock::now() - loop_start;

			if (speed != timescale::NOSLEEP) {
				dt_ms_t wait_time = per_frame - dt_us;

				if (wait_time > dt_ms_t::zero()) {
					SDL_Delay(wait_time.count());
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
