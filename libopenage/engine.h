// Copyright 2013-2020 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <memory>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <vector>
#include <QObject>

#include "log/log.h"
#include "log/file_logsink.h"
// pxd: from libopenage.cvar cimport CVarManager
#include "cvar/cvar.h"
#include "gui/engine_info.h"
#include "handlers.h"
#include "job/job_manager.h"
#include "input/action.h"
#include "options.h"
#include "presenter/presenter.h"
#include "util/externalprofiler.h"
#include "util/path.h"
#include "util/profiler.h"
#include "unit/selection.h"
#include "util/strings.h"


/**
 * Main openage namespace to store all things that make the have to do with the game.
 *
 * Game entity management, graphics drawing, gui stuff, input handling etc.
 * So basically everything that makes the game work lies in here...
 */
namespace openage {

namespace gui {
class GuiItemLink;
} // openage::gui

namespace presenter {
class LegacyDisplay;
} // openage::presenter


/**
 * Qt signals for the engine.
 */
class EngineSignals : public QObject {
	Q_OBJECT

public:
signals:
	void global_binds_changed(const std::vector<std::string>& global_binds);
};


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
	 * return this engine's job manager.
	 *
	 * TODO: remove ptr access
	 */
	job::JobManager *get_job_manager();

	/**
	 * return this engine's cvar manager.
	 */
	cvar::CVarManager &get_cvar_manager();

	/**
	 * current engine state variable.
	 * to be set to false to stop the engine loop.
	 */
	bool running;

	/**
	 * profiler used by the engine
	 */
	util::ExternalProfiler external_profiler;
private:
	/**
	 * Run-mode of the engine, this determines the basic modules to be loaded.
	 */
	mode mode;

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
	 * Presenter module: encapsulates all user interaction and graphics.
	 */
	std::shared_ptr<presenter::Presenter> presenter;

	/**
	 * the engine's job manager, for asynchronous background task queuing.
	 */
	job::JobManager job_manager;


	/**
	 * This stores information to be accessible from the QML engine.
	 *
	 * Information in there (such as a pointer to the this engine)
	 * is then usable from within qml files, after some additional magic.
	 */
	gui::EngineQMLInfo qml_info;

	/**
	 * the engine's cvar manager.
	 */
	std::shared_ptr<cvar::CVarManager> cvar_manager;


	/**
	 * the engines profiler
	 */
	util::Profiler profiler;

	/**
	 * Logsink to store messages to the filesystem.
	 */
	std::unique_ptr<log::FileSink> logsink_file;

	/**
	 * old, deprecated, and initial implementation of the renderer and game simulation.
	 * TODO: remove
	 */
	std::unique_ptr<presenter::LegacyDisplay> old_display;

public:
	/**
	 * Signal emitting capability for the engine.
	 */
	EngineSignals gui_signals;

	/**
	 * Link to the Qt GUI.
	 */
	gui::GuiItemLink *gui_link;
};

} // namespace openage
