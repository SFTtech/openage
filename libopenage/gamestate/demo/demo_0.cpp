// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "demo_0.h"

#include "cvar/cvar.h"
#include "gamestate/simulation.h"
#include "time/time_loop.h"


namespace openage::gamestate::tests {

void simulation_demo_0(const util::Path &path) {
	auto cvar = std::make_shared<cvar::CVarManager>(path);
	auto time_loop = std::make_shared<time::TimeLoop>();
	auto simulation = std::make_shared<GameSimulation>(path, cvar, time_loop);

	simulation->set_modpacks({"engine"});

	simulation->start();
}

} // namespace openage::gamestate::tests
