// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <chrono>

#include <unistd.h>

#include "gamestate.h"
#include "physics.h"
#include "gui.h"
#include "aicontroller.h"

typedef std::chrono::high_resolution_clock Clock;

namespace openage {
namespace tubepong {

int demo() {
	// Restart forever
	tubepong::Gui gui;
	tubepong::Physics phys;
	tubepong::AIInput ai;
	bool running = true;

	while (running) {
		tubepong::PongState state;
		tube::tube_time_t now = 1;

		state.p1.lives.set_drop(now, 3);
		state.p1.id = 0;
		state.p1.size.set_drop(now, 4);
		state.p2.lives.set_drop(now, 3);
		state.p2.id = 1;
		state.p2.size.set_drop(now, 4);

		auto init_speed = util::Vector<2>(
				((rand() % 2) * 2 - 1) * (0.1f + rand() % 4) / 70.f,
				0.01f * (rand() % 100) / 70.f);

		gui.draw(state, now); //update gui related parameters

		state.ball.speed.set_drop(now, init_speed);
		state.ball.position.set_drop(now, state.resolution * 0.5);
		state.p1.position.set_drop(now, state.resolution[1] / 2);
		state.p2.position.set_drop(now, state.resolution[1] / 2);

		gui.draw(state, now); //initial drawing with corrected ball

		auto loop_start = Clock::now();
		now += 1;
		std::cout << "p1: " << state.p1.lives.get(now) << " p2 " << state.p2.lives.get(now) << std::endl;


		while (state.p1.lives.get(now) > 0 && state.p2.lives.get(now) > 0) {
#ifdef HUMAN
			phys.processInput(
				state,
				state.p1,
				gui.getInputs(state.p1),
				now
			);
#else
			gui.getInputs(state.p1);
			phys.processInput(
				state,
				state.p1,
				ai.getInputs(state.p1, state.ball, now),
				now
			);
#endif
			phys.processInput(
				state,
				state.p2,
				ai.getInputs(state.p2, state.ball, now),
				now
			);

			state.p1.y = 0;
			state.p2.y = state.resolution[0]-1;

			phys.update(state, now);

			gui.draw(state, now);
			usleep(40000);

			double dt = std::chrono::duration_cast<std::chrono::milliseconds>((Clock::now() - loop_start)).count();
			now += dt;
		//	now += 40;
			loop_start = Clock::now();
		}
	}
	return 0;
}

}} // openage::tubepong
