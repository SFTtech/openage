// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "tests.h"

#include "demo/interactive/interactive.h"
#include "demo/pong/pong.h"
#include "demo/presenter/presenter.h"


namespace openage::main::tests {


void engine_demo(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0: {
		pong::main(path);
	} break;

	case 1: {
		presenter::demo(path);
	} break;

	case 2: {
		interactive::demo(path);
	} break;

	default:
		throw Error(ERR << "unknown engine demo " << demo_id << " requested.");
	}
}

} // namespace openage::main::tests
