// Copyright 2013-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <SDL2/SDL.h>

#include "buf.h"
#include "../handlers.h"
#include "../coord/pixel.h"
#include "../input/input_manager.h"
#include "../util/color.h"
#include "../renderer/font/font.h"
#include "../gamedata/color.gen.h"

namespace openage {

class Engine;

/**
 * In-game console subsystem. Featuring a full terminal emulator.
 */
namespace console {

class Console : InputHandler, TickHandler, HudHandler, ResizeHandler {

public:
	Console(Engine *engine);
	~Console();

	/**
	 * load the consoles color table
	 */
	void load_colors(std::vector<gamedata::palette_color> &colortable);

	/**
	 * register this console to the engine.
	 * this leads to the drawing calls, and input handling.
	 */
	void register_to_engine();

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
	bool on_resize(coord::viewport_delta new_size) override;

protected:
	Engine *engine;

public:
	coord::camhud bottomleft;
	coord::camhud topright;
	coord::camhud charsize;

	std::vector<util::col> termcolors;

	bool visible;

	Buf buf;
	renderer::Font font;

	input::InputContext input_context;

	// the command state
	std::string command;
};

}} // openage::console
