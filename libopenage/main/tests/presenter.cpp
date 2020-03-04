// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#include "presenter.h"

#include "../../presenter/presenter.h"


namespace openage::main::tests::presenter {

void demo(const util::Path &path) {
	log::log(INFO << "launching presenter test...");

	openage::presenter::Presenter presenter{path};
	presenter.run();
}

} // nameopenage::main::tests::presenter
