// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <cstdint>

#include <QObject>

#include "log/log.h"
#include "log/file_logsink.h"
#include "audio/audio_manager.h"
#include "coord/camgame.h"
#include "coord/vec2f.h"
#include "coord/phys3.h"
#include "coord/window.h"
// pxd: from libopenage.cvar cimport CVarManager
#include "cvar/cvar.h"
#include "game_singletons_info.h"
#include "handlers.h"
#include "input/action.h"
// pxd: from libopenage.input.input_manager cimport InputManager
#include "input/input_manager.h"
#include "job/job_manager.h"
#include "options.h"
#include "renderer/text.h"
#include "renderer/window.h"
#include "screenshot.h"
#include "util/dir.h"
#include "util/externalprofiler.h"
#include "util/fps.h"
#include "util/profiler.h"
#include "unit/selection.h"
#include "screenshot.h"


namespace openage {

namespace gui {
class GuiBasic;
class GuiItemLink;
} // openage::gui

namespace renderer {
class Font;
class FontManager;
class TextRenderer;
class Renderer;
class Window;
} // openage::renderer

namespace job {
class JobManager;
} // openage::job

class DrawHandler;
class TickHandler;
class ResizeHandler;

class Font;
class GameMain;

class GameSpec;
class Generator;
class Player;
class ScreenshotManager;


struct coord_data {
	coord::window window_size{800, 600};
	coord::phys3 camgame_phys{10 * coord::settings::phys_per_tile, 10 * coord::settings::phys_per_tile, 0};
	coord::window camgame_window{400, 300};
	coord::window camhud_window{0, 600};
	coord::camgame_delta tile_halfsize{48, 24};  // TODO: get from convert script
};

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
 *     InputManager get_input_manager() except +
 *     CVarManager get_cvar_manager() except +
 *     @staticmethod
 *     Engine &get() except +
 */
class Engine : public ResizeHandler, public options::OptionNode {
	friend class GameMain;
private:
	/**
	 * global engine singleton instance.
	 *
	 * TODO: use unique_ptr again, but that segfaults in freetype
	 *       because of a wrong deinit-order.
	 */
	static Engine *instance;

public:
	/**
	 * Returns a pointer to the engines coordinate data.
	 */
	static coord_data* get_coord_data();

	/**
	 * singleton constructor, use this to create the engine instance.
	 */
	static void create(util::Dir *data_dir, int32_t fps_limit, bool gl_debug, const char *windowtitle);

	/**
	 * singleton destructor, use when the program is shutting down.
	 */
	static void destroy();

	/**
	 * singleton instance fetcher.
	 * @returns the pointer to the global engine instance.
	 */
	static Engine &get();

private:
	/**
	 * engine initialization method.
	 * opens a window and initializes the OpenGL context.
	 */
	Engine(util::Dir *data_dir, int32_t fps_limit, bool gl_debug, const char *windowtitle);

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

	// log sinks
	std::unique_ptr<log::FileSink> logsink_file;

public:
	/**
	 * engine destructor, cleans up memory etc.
	 * deletes opengl context, the SDL window, and engine variables.
	 */
	virtual ~Engine();

	/**
	 * starts the engine loop.
	 */
	void run();

	/**
	 * enqueues the stop of the main loop.
	 */
	void stop();

	/**
	 * window resize handler function.
	 * recalculates opengl settings like viewport and projection matrices.
	 */
	bool on_resize(coord::window new_size) override;

	void start_game(const Generator &generator);
	void start_game(std::unique_ptr<GameMain> game);
	void end_game();

	/**
	 * draw the current frames per second number on screen.
	 * save the current framebuffer to a given png file.
	 * @param filename the file where the picture will be saved to.
	 */
	void save_screenshot(const char* filename);

	/**
	 * Draw the game version and the current FPS on screen.
	 */
	bool draw_debug_overlay();

	/**
	 * register a new input event handler, run for each input event.
	 */
	void register_input_action(InputHandler *handler);

	/**
	 * register a tick action, executed upon engine tick.
	 */
	void register_tick_action(TickHandler *handler);

	/**
	 * register a hud drawing handler, drawn in hud coordinates.
	 */
	void register_drawhud_action(HudHandler *handler);

	/**
	 * register a draw handler, run in game coordinates.
	 */
	void register_draw_action(DrawHandler *handler);

	/**
	 * register a resize handler, run when the window size changes.
	 */
	void register_resize_action(ResizeHandler *handler);

	/**
	 * return the data directory where the engine was started from.
	 */
	util::Dir *get_data_dir();

	/**
	 * return currently running game or null if a game is not
	 * currently running
	 */
	GameMain *get_game();

	/**
	 * return this engine's job manager.
	 */
	job::JobManager *get_job_manager();

	/**
	 * return this engine's audio manager.
	 */
	audio::AudioManager &get_audio_manager();

	/**
	* return this engine's screenshot manager.
	*/
	ScreenshotManager *get_screenshot_manager();

	/**
	* return this engine's action manager.
	*/
	input::ActionManager &get_action_manager();

	/**
	* return this engine's cvar manager.
	*/
	cvar::CVarManager &get_cvar_manager();

	/**
	* return this engine's keybind manager.
	*/
	input::InputManager &get_input_manager();

	/**
	* return this engine's unit selection.
	*/
	UnitSelection *get_unit_selection();

	/**
	* send keybindings help string to gui.
	*/
	void announce_global_binds();

	/**
	 * return this engine's text renderer.
	 */
	renderer::TextRenderer *get_text_renderer();

	/**
	 * return the number of nanoseconds that have passed
	 * for rendering the last frame.
	 *
	 * use that for fps-independent input actions.
	 */
	int64_t lastframe_duration_nsec();

	/**
	 * render text with the at a position with specified font size
	 */
	void render_text(coord::window position, size_t size, const char *format, ...) __attribute__((format (printf, 4, 5)));

	/**
	 * move the phys3 camera incorporated in the engine
	 */
	void move_phys_camera(float x, float y, float amount = 1.0);

	/**
	 * current engine state variable.
	 * to be set to false to stop the engine loop.
	 */
	bool running;

	/**
	 * FPS and game version are drawn when this is true.
	 */
	options::Var<bool> drawing_debug_overlay;

	/**
	* this allows to disable drawing of every registered hud.
	*/
	options::Var<bool> drawing_huds;

	/**
	 * Holds the data for the coord system.
	 */
	coord_data* engine_coord_data;

	/**
	 * profiler used by the engine
	 */
	util::ExternalProfiler external_profiler;

private:
	/**
	 * main engine loop function.
	 * this will be looped once per frame when the game is running.
	 *
	 * the loop invokes fps counting, SDL event handling,
	 * view translation, and calling the main draw_method.
	 */
	void loop();

	/**
	 * the current data directory for the engine.
	 */
	util::Dir *data_dir;

	/**
	 * how many nanoseconds are in a frame (1e9 / fps_limit).
	 * 0 if there is no fps limit.
	 */
	uint64_t ns_per_frame;

	/**
	 * input event processor objects.
	 * called for each captured sdl input event.
	 */
	std::vector<InputHandler *> on_input_event;

	/**
	 * run on every engine tick, after input handling, before rendering
	 */
	std::vector<TickHandler *> on_engine_tick;

	/**
	 * run every time the game is being drawn,
	 * with the renderer set to the camgame system
	 */
	std::vector<DrawHandler *> on_drawgame;

	/**
	 * run every time the hud is being drawn,
	 * with the renderer set to the camhud system
	 */
	std::vector<HudHandler *> on_drawhud;

	/**
	 * list of handlers that are executed upon a resize event.
	 */
	std::vector<ResizeHandler *> on_resize_handler;

	/**
	 * the currently running game
	 */
	std::unique_ptr<GameMain> game;

	/**
	 * pass engine pointer to gui
	 */
	gui::GameSingletonsInfo singletons_info;

	/**
	 * the frame counter measuring fps.
	 */
	util::FrameCounter fps_counter;

	/**
	* the engine's screenshot manager.
	*/
	std::unique_ptr<ScreenshotManager> screenshot_manager;

	/**
	 * the engine's cvar manager.
	 */
	cvar::CVarManager cvar_manager;

	/**
	 * the engine's action manager.
	 */
	input::ActionManager action_manager;

	/**
	 * the engine's audio manager.
	 */
	audio::AudioManager audio_manager;

	/**
	 * the engine's job manager, for asynchronous background task queuing.
	 */
	std::unique_ptr<job::JobManager> job_manager;

	/**
	 * the engine's keybind manager.
	 */
	input::InputManager input_manager;

	/**
	* the engine's unit selection.
	*/
	std::unique_ptr<UnitSelection> unit_selection;

	/**
	 * the text fonts to be used for (can you believe it?) texts.
	 * maps fontsize -> font
	 */
	std::unordered_map<int, renderer::Font *> fonts;

	/**
	 * The render window. Everything is drawn in here.
	 * Also contains the context.
	 */
	std::unique_ptr<renderer::Window> window;

	/**
	 * the gui binding
	 */
	std::unique_ptr<gui::GuiBasic> gui;

	/**
	 * The renderer. Accepts all tasks to be drawn on screen.
	 */
	std::unique_ptr<renderer::Renderer> renderer;

	/**
	 * The font manager to provide different sized and styled fonts.
	 */
	std::unique_ptr<renderer::FontManager> font_manager;

	/**
	 * The engine's text renderer. To be integrated into the main renderer.
	 */
	std::unique_ptr<renderer::TextRenderer> text_renderer;

public:
	EngineSignals gui_signals;

	gui::GuiItemLink *gui_link;

	/**
	 * the engines profiler
	 */
	util::Profiler profiler;
};

} // namespace openage
