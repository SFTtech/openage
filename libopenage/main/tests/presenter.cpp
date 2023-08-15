// Copyright 2019-2023 the openage authors. See copying.md for legal info.

#include "presenter.h"

#include "log/log.h"
#include "presenter/presenter.h"
#include "time/time_loop.h"


namespace openage::main::tests::presenter {

void demo(const util::Path &path) {
	log::log(INFO << "launching presenter test...");

	auto time_loop = std::make_shared<openage::time::TimeLoop>();
	openage::presenter::Presenter presenter{path};
	presenter.set_time_loop(time_loop);
	presenter.run();
}

} // namespace openage::main::tests::presenter
