// Copyright 2020-2020 the openage authors. See copying.md for legal info.

#pragma once

#include "../../audio/audio_manager.h"
// pxd: from libopenage.coord.coordmanager cimport CoordManager
#include "../../coord/coordmanager.h"
#include "../../error/gl_debug.h"
#include "../../handlers.h"
// pxd: from libopenage.input.input_manager cimport InputManager
#include "../../input/input_manager.h"
#include "../../options.h"
#include "../../renderer/font/font.h"
#include "../../renderer/font/font_manager.h"
#include "../../renderer/text.h"
#include "../../screenshot.h"
#include "../../util/fps.h"
#include "../../util/path.h"
#include "../../util/timing.h"


namespace openage {

class DrawHandler;
class TickHandler;
class ResizeHandler;

class Generator;
class GameSpec;
class GameMain;
class UnitSelection;

namespace gui {
class GUI;
} // gui

namespace renderer {

class Font;
class FontManager;
class TextRenderer;
class Color;

} // renderer


namespace presenter {

/**
 * Temporary container class for the legacy renderer implementation.
 */
class LegacyDisplay final : public ResizeHandler,
                            public options::OptionNode {
public:
	LegacyDisplay(const util::Path &path);

	~LegacyDisplay();

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
	 * order: 1 above, -1 below
	 */
	void register_drawhud_action(HudHandler *handler, int order = 1);

	/**
	 * register a draw handler, run in game coordinates.
	 */
	void register_draw_action(DrawHandler *handler);

	/**
	 * register a resize handler, run when the window size changes.
	 */
	void register_resize_action(ResizeHandler *handler);

	/**
	 * window resize handler function.
	 * recalculates opengl settings like viewport and projection matrices.
	 */
	bool on_resize(coord::viewport_delta new_size) override;

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
	time_nsec_t lastframe_duration_nsec() const;

	/**
	 * send keybindings help string to gui.
	 */
	void announce_global_binds();

	/**
	 * return this engine's unit selection.
	 */
	UnitSelection *get_unit_selection();

	/**
	 * render text at a position with the specified font size
	 */
	void render_text(coord::viewport position, size_t size, const renderer::Color &color, const char *format, ...) ATTRIBUTE_FORMAT(5, 6);

	/**
	 * move the phys3 camera incorporated in the engine
	 */
	void move_phys_camera(float x, float y, float amount=1.0);

	/**
	 * Start a game with the given game generator.
	 */
	void start_game(const Generator &generator);

	/**
	 * Start a game with the given initialized game.
	 */
	void start_game(std::unique_ptr<GameMain> &&game);

	/**
	 * Stop the running game.
	 */
	void end_game();

	/**
	 * return currently running game or null if a game is not
	 * currently running
	 */
	GameMain *get_game();

	/**
	 * legacy engine loop function.
	 * this will be looped once per frame when the game is running.
	 *
	 * the loop invokes fps counting, SDL event handling,
	 * view translation, and calling the main draw_method.
	 */
	void loop();

public:

	/**
	 * return this engine's audio manager.
	 */
	audio::AudioManager &get_audio_manager();

	/**
	 * return this engine's screenshot manager.
	 */
	ScreenshotManager &get_screenshot_manager();

	/**
	 * return this engine's action manager.
	 */
	input::ActionManager &get_action_manager();

	/**
	 * return this engine's keybind manager.
	 */
	input::InputManager &get_input_manager();

	/**
	 * FPS and game version are drawn when this is true.
	 */
	options::Var<bool> drawing_debug_overlay;

	/**
	 * this allows to disable drawing of every registered hud.
	 */
	options::Var<bool> drawing_huds;

	/**
	 * The coordinate state.
	 */
	coord::CoordManager coord;

private:
	/**
	 * the currently running game
	 */
	std::unique_ptr<GameMain> game;

	/**
	 * how many nanoseconds are in a frame (1e9 / fps_limit).
	 * 0 if there is no fps limit.
	 */
	time_nsec_t ns_per_frame;

	/**
	 * input event processor objects.
	 * called for each captured sdl input event.
	 */
	std::vector<InputHandler *> on_input_event;

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
	 * run on every engine tick, after input handling, before rendering
	 */
	std::vector<TickHandler *> on_engine_tick;

	/**
	 * the frame counter measuring fps.
	 */
	util::FrameCounter fps_counter;

	/**
	 * the engine's screenshot manager.
	 */
	ScreenshotManager screenshot_manager;

	/**
	 * the engine's action manager.
	 */
	input::ActionManager action_manager;

	/**
	 * the engine's audio manager.
	 */
	audio::AudioManager audio_manager;

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
	 * SDL window where everything is displayed within.
	 */
	SDL_Window *window;

	/**
	 * SDL OpenGL context, we'll only have one,
	 * but it would allow having multiple ones.
	 *
	 * This is actually a void * but sdl2 thought it was a good idea to
	 * name it like a differently.
	 */
	SDL_GLContext glcontext;

	/**
	 * Qt GUI system
	 */
	std::unique_ptr<gui::GUI> gui;

	/**
	 * ttf font loading manager
	 */
	std::unique_ptr<renderer::FontManager> font_manager;

	/**
	 * 2d text renderer
	 */
	std::unique_ptr<renderer::TextRenderer> text_renderer;
};

}} // openage::presenter
