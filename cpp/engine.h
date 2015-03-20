// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_ENGINE_H_
#define OPENAGE_ENGINE_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>

#include "log/log.h"
#include "log/file_logsink.h"
#include "audio/audio_manager.h"
#include "coord/camgame.h"
#include "coord/vec2f.h"
#include "coord/phys3.h"
#include "coord/window.h"
#include "font.h"
#include "handlers.h"
#include "input.h"
#include "job/job_manager.h"
#include "util/dir.h"
#include "util/fps.h"
#include "screenshot.h"

namespace openage {

class DrawHandler;
class TickHandler;
class InputHandler;
class ResizeHandler;
class CoreInputHandler;

class GameMain;

/**
 * main engine container.
 *
 * central foundation for everything the openage engine is capable of.
 */
class Engine : public ResizeHandler {
	friend class GameMain;
private:
	/**
	 * global engine singleton instance.
	 *
	 * TODO: use unique_ptr again, but that segfaults in ftgl/freetype
	 *       because of a wrong deinit-order.
	 */
	static Engine *instance;

public:
	/**
	 * singleton constructor, use this to create the engine instance.
	 */
	static void create(util::Dir *data_dir, const char *windowtitle);

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
	Engine(util::Dir *data_dir, const char *windowtitle);

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
	Engine &operator=(Engine &&other);

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
	bool on_resize(coord::window new_size);

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
	ScreenshotManager &get_screenshot_manager();

	/**
	 * return the core input handler of the engine.
	 */
	CoreInputHandler &get_input_handler();

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
	void render_text(coord::window position, size_t size, const char *format, ...);

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
	bool drawing_debug_overlay;

	/**
	* this allows to disable drawing of every registered hud.
	*/
	bool drawing_huds;

	/**
	 * size of the game window, in coord_sdl.
	 */
	coord::window window_size;

	/**
	 * position of the game camera, in the phys3 system.
	 * (the position that it is rendered at camgame {0, 0})
	 */
	coord::phys3 camgame_phys;

	/**
	 * position of the game camera, in the window system.
	 * (the position where the camgame {0, 0} is rendered)
	 */
	coord::window camgame_window;

	/**
	 * position of the hud camera, in the window system.
	 * (the position where camhud {0, 0} is rendered)
	 */
	coord::window camhud_window;

	/**
	 * half the size of one terrain diamond tile, in camgame.
	 */
	coord::camgame_delta tile_halfsize;

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
	 * the core engine input handler.
	 * additional input handlers may be registered with
	 * `register_input_action`
	 */
	CoreInputHandler input_handler;

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
	 * the frame counter measuring fps.
	 */
	util::FrameCounter fps_counter;

	/**
	* the engine's screenshot manager.
	*/
	ScreenshotManager screenshot_manager;

	/**
	 * the engine's audio manager.
	 */
	audio::AudioManager audio_manager;

	/**
	 * the engine's job manager, for asynchronous background task queuing.
	 */
	job::JobManager *job_manager;

	/**
	 * the text fonts to be used for (can you believe it?) texts.
	 * maps fontsize -> font
	 */
	std::unordered_map<int, std::unique_ptr<Font>> fonts;

	/**
	 * SDL window where everything is displayed within.
	 */
	SDL_Window *window;

	/**
	 * SDL OpenGL context, we'll only have one,
	 * but it would allow having multiple ones.
	 */
	SDL_GLContext glcontext;
};

} // namespace openage

#endif
