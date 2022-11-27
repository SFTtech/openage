// Copyright 2013-2022 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage {

namespace cvar {
class CVarManager;
}

namespace event {
class Loop;
}

namespace gamestate {
class Game;
}

namespace renderer {
class RenderFactory;
}

namespace engine {


/**
 * Gameplay subsystem of the engine.
 *
 * Manages the state of the game world and the event loop.
 */
class Engine final {
public:
	enum class mode {
		LEGACY,
		HEADLESS,
		FULL,
	};

	/**
	 * engine initialization method.
	 * starts the engine subsystems depending on the requested run mode.
	 */
	Engine(mode mode,
	       const util::Path &root_dir,
	       const std::shared_ptr<cvar::CVarManager> &cvar_manager);

	/**
	 * engine copy constructor.
	 */
	Engine(const Engine &copy) = delete;

	/**
	 * engine assignment operator.
	 */
	Engine &operator=(const Engine &copy) = delete;

	/**
	 * engine move constructor.
	 */
	Engine(Engine &&other) = delete;

	/**
	 * engine move operator.
	 */
	Engine &operator=(Engine &&other) = delete;

	/**
	 * engine destructor, cleans up memory etc.
	 * deletes opengl context, the SDL window, and engine variables.
	 */
	~Engine();

	/**
	 * starts the engine loop.
	 */
	void run();

	/**
	 * enqueues the stop of the main loop.
	 */
	void stop();

	/**
	 * return the data directory where the engine was started from.
	 */
	const util::Path &get_root_dir();

	/**
	 * return this engine's cvar manager.
	 */
	std::shared_ptr<cvar::CVarManager> get_cvar_manager();

	/**
	 * Attach a renderer to the engine.
	 * 
	 * @param factory Factory for creating render entities.
	 */
	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

	/**
	 * current engine state variable.
	 * to be set to false to stop the engine loop.
	 */
	bool running;

private:
	/**
	 * Run-mode of the engine, this determines the basic modules to be loaded.
	 */
	mode run_mode;

	/**
	 * The engine root directory.
	 * Uses the openage fslike path abstraction that can mount paths into one.
	 *
	 * This means that this path does simulataneously lead to global assets,
	 * home-folder-assets, settings, and basically the whole filesystem access.
	 *
	 * TODO: move this to a settings class, which then also hosts cvar and the options system.
	 */
	util::Path root_dir;

	/**
	 * the engine's cvar manager.
	 */
	std::shared_ptr<cvar::CVarManager> cvar_manager;

	/**
	 * Event loop for creating and sending events.
	 */
	std::shared_ptr<event::Loop> event_loop;

	// Something that manages the clock
	// auto time_manager;

	// TODO: The game run by the engine
	std::shared_ptr<gamestate::Game> game;
};

} // namespace engine
} // namespace openage