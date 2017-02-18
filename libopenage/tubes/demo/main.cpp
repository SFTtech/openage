#include <chrono>

#include <ncurses.h>

#include <unistd.h>

#include "gamestate.h"
#include "physics.h"
#include "gui.h"
#include "aicontroller.h"

using namespace openage;

typedef std::chrono::high_resolution_clock Clock;

int main() {
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

		auto init_speed = vertex2f(
				((rand() % 2) * 2 - 1) * (0.1f + rand() % 4) / 70.f,
				0.01f * (rand() % 100) / 70.f);

		gui.draw(state, now); //update gui related parameters

		state.ball.speed.set_drop(now, init_speed);
		state.ball.position.set_drop(now, state.resolution * 0.5);
		state.p1.position.set_drop(now, state.resolution[1] / 2);
		state.p2.position.set_drop(now, state.resolution[1] / 2);

		gui.draw(state, now); //initial drawing with corrected ball

		auto loop_start = Clock::now();
//		std::cout << "1" << std::endl;
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

//			std::cout <<  now << std::endl;
			gui.draw(state, now);
			usleep(10000);

		//	double dt = std::chrono::duration_cast<std::chrono::milliseconds>((Clock::now() - loop_start)).count();
			now += 10; //dt;
			loop_start = Clock::now();
		}
	}
}
