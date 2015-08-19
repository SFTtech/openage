// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_CONSOLE_CONSOLE_H_
#define OPENAGE_CONSOLE_CONSOLE_H_

#include <vector>
#include <SDL2/SDL.h>

#include "buf.h"
#include "../handlers.h"
#include "../coord/camhud.h"
#include "../engine.h"
#include "../input/input_manager.h"
#include "../util/color.h"
#include "../font.h"
#include "../gamedata/color.gen.h"
#include "../log/tail_logsink.h"

namespace openage {

/**
 * In-game console subsystem. Featuring a full terminal emulator.
 */
namespace console {

class Console : InputHandler, TickHandler, HudHandler, ResizeHandler {

public:
	Console();

	coord::camhud bottomleft;
	coord::camhud topright;
	coord::camhud charsize;

	std::vector<util::col> termcolors;

	bool visible;

	Buf buf;
	Font font;

	input::InputContext input_context;

	// the command state
	std::string command;

	/**
	 * the log sink used for the 'log-tail' command.
	 */
	std::unique_ptr<log::TailSink> tailsink;

	/**
	 * load the consoles color table
	 */
	void load_colors(std::vector<gamedata::palette_color> &colortable);

	/**
	 * register this console to the engine.
	 * this leads to the drawing calls, and input handling.
	 */
	void register_to_engine(Engine *engine);

	void set_visible(bool make_visible);

	/**
	 * prints the given text on the console.
	 */
	void write(const char *text);

	/**
	 * a temporary console command interpreter
	 */
	void interpret(const std::string &command);

	bool on_drawhud() override;
	bool on_tick() override;
	bool on_input(SDL_Event *event) override;
	bool on_resize(coord::window new_size) override;
};

}} // openage::console

#endif
