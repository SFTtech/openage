// Copyright 2019-2023 the openage authors. See copying.md for legal info.

#include "presenter.h"

#include "log/log.h"
#include "presenter/presenter.h"


namespace openage::main::tests::presenter {

void demo(const util::Path &path) {
	log::log(INFO << "launching presenter test...");

	openage::presenter::Presenter presenter{path};
	presenter.run();
}

} // namespace openage::main::tests::presenter
