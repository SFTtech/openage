// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include "tests/pong.h"


namespace openage::main::tests {


void engine_demo(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0:
		pong::main(path);
		break;

	default:
		throw Error(ERR << "unknown engine demo " << demo_id << " requested.");
	}
}

} // namespace openage::main::tests
