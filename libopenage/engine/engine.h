// Copyright 2013-2022 the openage authors. See copying.md for legal info.

#pragma once

#include "cvar/cvar.h"
#include "gui/engine_info.h"
#include "renderer/gui/qml_info.h"
#include "util/path.h"


/**
 * Main openage namespace to store all things that make the have to do with the game.
 *
 * Game entity management, graphics drawing, gui stuff, input handling etc.
 * So basically everything that makes the game work lies in here...
 */
namespace openage {
namespace engine {


/**
 * main engine container.
 *
 * central foundation for everything the openage engine is capable of.
 *
 * pxd:
 *
 * cppclass Engine:
 *
 *     InputManager &get_input_manager() except +
 *     CVarManager &get_cvar_manager() except +
 */
class Engine final {
public:
	enum class mode {
		LEGACY,
		HEADLESS,
		FULL,
	};

	Engine();

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

public:
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
	 * return this engine's qml info.
	 */
	renderer::gui::QMLInfo get_qml_info();

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
	 * This stores information to be accessible from the QML engine.
	 *
	 * Information in there (such as a pointer to the this engine)
	 * is then usable from within qml files, after some additional magic.
	 */
	renderer::gui::QMLInfo qml_info;
};

} // namespace engine
} // namespace openage