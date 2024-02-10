// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "demo_0.h"

#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"


namespace openage::path::tests {

void path_demo_0(const util::Path &path) {
	auto qtapp = std::make_shared<renderer::gui::GuiApplicationWithLogger>();

	renderer::opengl::GlWindow window("openage pathfinding test", 1024, 768, true);
	auto renderer = window.make_renderer();

	while (not window.should_close()) {
		qtapp->process_events();

		window.update();

		renderer->check_error();
	}
	window.close();
}

} // namespace openage::path::tests
