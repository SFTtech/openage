// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "tests.h"

#include "log/log.h"
#include "log/message.h"

#include "gamestate/demo/demo_0.h"


namespace openage::gamestate::tests {

void simulation_demo(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0:
		simulation_demo_0(path);
		break;

	default:
		log::log(MSG(err) << "Unknown renderer demo requested: " << demo_id << ".");
		break;
	}
}

} // namespace openage::gamestate::tests
