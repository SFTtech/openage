// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <memory>
#include <QObject>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <vector>

#include "log/file_logsink.h"
#include "log/log.h"
// pxd: from libopenage.cvar cimport CVarManager
#include "cvar/cvar.h"
#include "gui/engine_info.h"
#include "handlers.h"
#include "input/legacy/action.h"
#include "job/job_manager.h"
#include "options.h"
#include "unit/selection.h"
#include "util/externalprofiler.h"
#include "util/path.h"
#include "util/profiler.h"
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
} // namespace gui

/**
 * Qt signals for the engine.
 */
class EngineSignals : public QObject {
	Q_OBJECT

public:
signals:
	void global_binds_changed(const std::vector<std::string> &global_binds);
};


/**
 * main engine container.
 *
 * central foundation for everything the openage engine is capable of.
 *
 * pxd:
 *
 * cppclass LegacyEngine:
 *
 *     InputManager &get_input_manager() except +
 *     CVarManager &get_cvar_manager() except +
 */
class LegacyEngine final {
public:
	enum class mode {
		LEGACY,
		HEADLESS,
		FULL,
	};

	LegacyEngine();

	/**
	 * engine initialization method.
	 * starts the engine subsystems depending on the requested run mode.
	 */
	LegacyEngine(mode mode,
	             const util::Path &root_dir,
	             const std::shared_ptr<cvar::CVarManager> &cvar_manager);

	/**
	 * engine copy constructor.
	 */
	LegacyEngine(const LegacyEngine &copy) = delete;

	/**
	 * engine assignment operator.
	 */
	LegacyEngine &operator=(const LegacyEngine &copy) = delete;

	/**
	 * engine move constructor.
	 */
	LegacyEngine(LegacyEngine &&other) = delete;

	/**
	 * engine move operator.
	 */
	LegacyEngine &operator=(LegacyEngine &&other) = delete;

public:
	/**
	 * engine destructor, cleans up memory etc.
	 * deletes opengl context, the SDL window, and engine variables.
	 */
	~LegacyEngine();

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
	std::shared_ptr<cvar::CVarManager> get_cvar_manager();

	/**
	 * return this engine's qml info.
	 */
	gui::EngineQMLInfo get_qml_info();

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
