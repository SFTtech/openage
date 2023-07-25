// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "engine.h"

#include "log/log.h"

#include "cvar/cvar.h"
#include "gamestate/simulation.h"
#include "presenter/presenter.h"
#include "time/time_loop.h"


namespace openage::engine {

Engine::Engine(mode mode,
               const util::Path &root_dir,
               const std::vector<std::string> &mods) :
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

	this->simulation = std::make_shared<gamestate::GameSimulation>(this->root_dir,
	                                                               this->cvar_manager,
	                                                               this->time_loop);
	this->simulation->set_modpacks(mods);

	if (this->run_mode == mode::FULL) {
		this->presenter = std::make_shared<presenter::Presenter>(this->root_dir,
		                                                         this->simulation,
		                                                         this->time_loop);
	}

	this->threads.emplace_back([&]() {
		this->time_loop->run();

		this->time_loop.reset();
	});
	this->threads.emplace_back([&]() {
		this->simulation->run();

		this->simulation.reset();

		if (this->run_mode != mode::FULL) {
			this->running = false;
		}
	});

	if (this->run_mode == mode::FULL) {
		this->threads.emplace_back([&]() {
			this->presenter->run();

			// Make sure that the presenter gets destructed in the same thread
			// otherwise OpenGL complains about missing contexts
			this->presenter.reset();
			this->running = false;
		});
	}

	log::log(INFO << "Created " << this->threads.size() << " threads"
	              << " (" << std::jthread::hardware_concurrency() << " available)");
}

void Engine::loop() {
	while (this->running) {
		// TODO
	}
}

} // namespace openage::engine
