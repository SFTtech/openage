// Copyright 2018-2019 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>
#include <memory>

#include <nyan/nyan.h>

#include "aicontroller.h"
#include "gui.h"
#include "physics.h"
#include "../../error/error.h"
#include "../../event/loop.h"
#include "../../log/log.h"
#include "../../util/math_constants.h"
#include "../../util/path.h"


namespace openage::main::tests::pong {



using Clock = std::chrono::high_resolution_clock;
using namespace std::literals::chrono_literals;


enum class timescale {
	NOSLEEP,
	REALTIME,
	SLOW,
	FAST,
};

void main(const util::Path& path) {
	bool human_player = false;

	timescale speed = timescale::REALTIME;

	std::shared_ptr<nyan::Database> db = nyan::Database::create();

	db->load(
		"pong.nyan",
		[&path] (const std::string &filename) {
			// TODO: nyan should provide a file api that we can inherit from
			// then we can natively interface to the openage file abstraction
			// and do not need to read the whole file here.
			// ideally, the returned file lazily accesses all data through openage::util::File.
			return std::make_shared<nyan::File>(
				filename,
				(path / ("assets/nyan/" + filename)).open_r().read()
			);
		}
	);

	std::shared_ptr<nyan::View> dbview = db->new_view();
	std::shared_ptr<Gui> gui = std::make_shared<Gui>();

	bool running = true;

	while (running) {
		log::log(INFO << "initializing new pong game...");

		auto loop = std::make_shared<event::Loop>();
		curve::time_t now = 0;
		Physics phys;

		// the window size is fetched in here already,
		// so the resize callback doesn't need to be called for
		// a new state.
		auto state = std::make_shared<PongState>(loop, gui, dbview);

		gui->clear_resize_callbacks();
		gui->add_resize_callback(
			[&] (size_t w, size_t h) {
				log::log(INFO << "update pong area size=("
				         << w << "," << h << ")...");

				state->area_size->set_last(now, {w, h});
			}
		);

		// process one round of window events, mainly to get the initial screen size.
		gui->update();

		Physics::init(state, loop, now);

		// initialize player properties
		state->p1->lives->set_last(now, 3);
		state->p1->size->set_last(now, 200);

		state->p2->lives->set_last(now, 3);
		state->p2->size->set_last(now, 200);

		// initialize the game enqueuing a physics reset should happen.
		log::log(INFO << "initializing the physics state...");
		std::vector<PongEvent> start_event{PongEvent{0, PongEvent::START}};
		phys.process_input(state, state->p1, start_event, loop, now);
		loop->reach_time(now, state);

		std::vector<PongEvent> inputs;

		log::log(INFO << "starting game loop...");

		// this is the game loop, running while both players live!
		while (running and
		       state->p1->lives->get(now) > 0 and
		       state->p2->lives->get(now) > 0) {

			log::log(DBG << "=================== LOOPING ====================");

			auto loop_start = Clock::now();

			// process the input for both players
			// player 1 can be AI or human.

			if (human_player) {
				phys.process_input(state, state->p1,
				                   inputs, loop, now);
			}
			else {
				phys.process_input(state, state->p1,
				                   get_ai_inputs(state->p1, state->ball,
				                                 state->area_size, now, false),
				                   loop, now);
			}

			phys.process_input(state, state->p2,
			                   get_ai_inputs(state->p2, state->ball,
			                                 state->area_size, now, true),
			                   loop, now);

			// evaluate the event queue to reach the desired game time!
			loop->reach_time(now, state);

			// draw the new state
			gui->draw(state, now);

			// TODO: print event queue from loop->get_queue().get_event_queue().get_sorted_events()
			//       and show event->get_time and event->get_eventhandler()->id()

			// get the input after the current frame, because the get_inputs
			// implicitly updates the screen. if this is done too early,
			// the screen will be updated to be empty -> flickering.
			// TODO: take terminal input without ncurses, if on tty?
			//       e.g. 'q' to close it?
			inputs = gui->get_inputs(state->p1);

			if (gui->exit_requested) {
				running = false;
			}

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


} // openage::main::tests::pong
