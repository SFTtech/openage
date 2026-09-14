// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "engine.h"

#include "log/log.h"
#include "log/message.h"

#include "cvar/cvar.h"
#include "gamestate/simulation.h"
#include "presenter/presenter.h"
#include "time/time_loop.h"


namespace openage::engine {

Engine::Engine(mode mode,
               const util::Path &root_dir,
               const std::vector<std::string> &mods,
               const renderer::window_settings &window_settings) :
	running{true},
	run_mode{mode},
	root_dir{root_dir},
	threads{} {
	log::log(INFO
	         << "launching engine with root directory"
	         << root_dir);

	// read and apply the configuration files
	this->cvar_manager = std::make_shared<cvar::CVarManager>(this->root_dir["cfg"]);
	cvar_manager->load_all();

	// time loop
	this->time_loop = std::make_shared<time::TimeLoop>();

	// game simulation
	this->simulation = std::make_shared<gamestate::GameSimulation>(this->root_dir,
	                                                               this->cvar_manager,
	                                                               this->time_loop);
	this->simulation->set_modpacks(mods);

	// presenter (optional)
	if (this->run_mode == mode::FULL) {
		this->presenter = std::make_shared<presenter::Presenter>(this->root_dir,
		                                                         this->simulation,
		                                                         this->time_loop,
		                                                         window_settings);
	}
}

void Engine::loop() {
	// Spawn the time loop in a worker thread. The time loop advances the
	// simulation clock and runs independently of the main loop.
	this->threads.emplace_back([&]() {
		this->run_time_loop();
	});

	if (this->run_mode == mode::FULL) {
		// Run the simulation in a worker thread...
		this->threads.emplace_back([&]() {
			this->run_simulation();
		});

		log::log(INFO << "Using " << this->threads.size() + 1 << " threads "
		              << "(" << std::jthread::hardware_concurrency() << " available)");

		// ...while the presenter runs on the main thread. The presenter owns the
		// QGuiApplication, which Qt requires to be created and operated on the
		// main thread (on macOS, QGuiApplication setup on a non-main thread is
		// an API misuse that the platform plugin rejects). This is also why the
		// simulation cannot stay on the main thread in FULL mode: the main
		// thread must be free to drive the presenter.
		this->presenter->run();
		log::log(MSG(info) << "presenter exited");
		// Make sure that the presenter gets destructed in the same thread,
		// otherwise OpenGL complains about missing contexts.
		this->presenter.reset();
		this->running = false;
	}
	else {
		log::log(INFO << "Using " << this->threads.size() + 1 << " threads "
		              << "(" << std::jthread::hardware_concurrency() << " available)");

		// In headless mode there is no presenter, so the main thread is free to
		// run the simulation directly.
		this->run_simulation();
	}
}

void Engine::run_time_loop() {
	this->time_loop->run();
	this->time_loop.reset();
}

void Engine::run_simulation() {
	// Run the main game simulation loop:
	this->simulation->run();
	// After stopping, clean up the simulation
	this->simulation.reset();
	if (this->run_mode != mode::FULL) {
		this->running = false;
	}
}

} // namespace openage::engine
