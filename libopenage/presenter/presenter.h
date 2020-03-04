// Copyright 2019-2020 the openage authors. See copying.md for legal info.

#pragma once

#include "../renderer/window.h"
#include "../util/path.h"


namespace openage::presenter {

class Presenter {
public:
	Presenter(const util::Path &path);

	void run();

protected:
	util::Path root_dir;

	std::shared_ptr<renderer::Window> window;
	std::shared_ptr<renderer::Renderer> renderer;
};

} // openage::presenter
