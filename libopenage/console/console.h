// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include "../coord/pixel.h"
#include "../gamedata/color_dummy.h"
#include "../handlers.h"
#include "../input/legacy/input_manager.h"
#include "../presenter/legacy/legacy.h"
#include "../renderer/font/font.h"
#include "../util/color.h"
#include "buf.h"

namespace openage {

class LegacyEngine;

/**
 * In-game console subsystem. Featuring a full terminal emulator.
 */
namespace console {

class Console : InputHandler
	, TickHandler
	, HudHandler
	, ResizeHandler {
public:
	Console(presenter::LegacyDisplay *renderer);
	~Console();

	/**
	 * load the consoles color table
	 */
	void load_colors(std::vector<gamedata::palette_color> &colortable);

	/**
	 * register this console to the renderer.
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
	presenter::LegacyDisplay *engine;

public:
	coord::camhud bottomleft;
	coord::camhud topright;
	coord::camhud charsize;

	std::vector<util::col> termcolors;

	bool visible;

	Buf buf;
	renderer::Font font;

	input::legacy::InputContext input_context;

	// the command state
	std::string command;
};

} // namespace console
} // namespace openage
