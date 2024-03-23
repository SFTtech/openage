// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "tests.h"

#include "log/log.h"

#include "pathfinding/demo/demo_0.h"
#include "pathfinding/demo/demo_1.h"


namespace openage::path::tests {

void path_demo(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0:
		path_demo_0(path);
		break;

	case 1:
		path_demo_1(path);
		break;

	default:
		log::log(MSG(err) << "Unknown pathfinding demo requested: " << demo_id << ".");
		break;
	}
}

} // namespace openage::path::tests
