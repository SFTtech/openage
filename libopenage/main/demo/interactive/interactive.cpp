// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "interactive.h"

#include <algorithm>

#include "log/log.h"

#include "assets/mod_manager.h"
#include "engine/engine.h"


namespace openage::main::tests::interactive {

void demo(const util::Path &path) {
	log::log(INFO << "Launching interactive demo...");

	// check for available modpacks
	auto modpack_folder = path / "assets" / "converted";
	auto mods = assets::ModManager::enumerate_modpacks(modpack_folder);

	// check if the engine modpack exists
	auto mod_engine = std::find_if(mods.begin(), mods.end(), [](const assets::ModpackInfo &mod) {
		return mod.id == "engine";
	});
	if (mod_engine == std::end(mods)) {
		throw Error{MSG(err) << "Modpack 'engine' not found.\n"
		                     << "You need to export it first by running "
		                     << "'python -m openage convert-export-api'"};
	}
	else {
		// remove the engine modpack from the mod selection
		// because it's not playable
		mods.erase(mod_engine);
	}

	// check if there are is at least one playable modpack
	if (mods.empty()) {
		throw Error{MSG(err) << "No playable modpacks found.\n"
		                     << "Try exporting some with "
		                     << "'python -m openage convert'"};
	}

	// select one of the modpacks
	std::cout << "Choose modpack to use:" << std::endl;
	for (size_t i = 0; i < mods.size(); ++i) {
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
