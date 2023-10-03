// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "main.h"

#include <memory>

#include "cvar/cvar.h"
#include "engine/engine.h"
#include "util/timer.h"

#include "renderer/window.h"
#include "renderer/enums.h"

namespace openage {

/*
 * This function is called from Cython, see main_cpp.pyx.
 *
 * This is the main entry point to the C++ part.
 */
int run_game(const main_arguments &args) {
	// TODO: store args.gl_debug as default in the cvar system.

	util::Timer timer;
	timer.start();

	// read and apply the configuration files
	auto cvar_manager = std::make_shared<cvar::CVarManager>(args.root_path["cfg"]);
	cvar_manager->load_all();

	// set engine run_mode
	openage::engine::Engine::mode run_mode = openage::engine::Engine::mode::FULL;
	if (args.headless) {
		run_mode = openage::engine::Engine::mode::HEADLESS;
	}
	renderer::WindowConfig window_config;
    window_config.width = args.window_config.width;
    window_config.height = args.window_config.height;
    window_config.vsync = args.window_config.vsync;

    // Determine the render mode based on the window-related arguments
    if (args.window_config.render_mode == renderer::RenderMode::FULLSCREEN) {
        window_config.render_mode = renderer::RenderMode::FULLSCREEN;
    } else if (args.window_config.render_mode == renderer::RenderMode::BORDERLESS) {
        window_config.render_mode = renderer::RenderMode::BORDERLESS;
    } else {
        window_config.render_mode = renderer::RenderMode::WINDOWED;
    }

	openage::engine::Engine engine{run_mode, args.root_path, args.mods, args.gl_debug, window_config};

	engine.loop();

	return 0;
}

} // namespace openage
