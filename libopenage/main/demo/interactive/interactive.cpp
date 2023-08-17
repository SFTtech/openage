// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "interactive.h"

#include "log/log.h"

#include "assets/mod_manager.h"
#include "engine/engine.h"


namespace openage::main::tests::interactive {

void demo(const util::Path &path) {
	log::log(INFO << "Launching interactive demo...");

	// check for available modpacks
	auto modpack_folder = path / "assets" / "converted";
	auto mods = assets::ModManager::enumerate_modpacks(modpack_folder);

	// select one of the modpacks
	std::cout << "Choose modpack to use:" << std::endl;
	for (size_t i = 0; i < mods.size(); ++i) {
		if (mods[i].id == "engine") {
			// skip the internal engine modpack because
			// it is loaded by default and not playable
			continue;
		}

		std::cout << "(" << i << ")"
				  << " " << mods[i].id
				  << std::endl;
	}
	std::cout << "> ";
	size_t mod_idx;
	std::cin >> mod_idx;

	// start the engine with the selected modpack
	auto engine = engine::Engine(engine::Engine::mode::FULL,
	                             path,
	                             {mods[mod_idx].id});

	// engine main thread has to loop, otherwise RAII frees up
	// the members of engine and we get allocation errors
	engine.loop();
}

} // namespace openage::main::tests::interactive
