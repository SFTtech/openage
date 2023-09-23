// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "../coord/pixel.h"
#include "../renderer/font/font.h"
#include "../util/color.h"
#include "buf.h"

namespace openage {

/**
 * In-game console subsystem. Featuring a full terminal emulator.
 *
 * TODO: Adapt to new engine subsystems.
 */
namespace console {

class Console {
public:
	Console(/* presenter::LegacyDisplay *display */);
	~Console();

	/**
	 * load the consoles color table
	 */
	// void load_colors(std::vector<gamedata::palette_color> &colortable);

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

	// bool on_drawhud() override;
	// bool on_tick() override;
	// bool on_input(SDL_Event *event) override;
	// bool on_resize(coord::viewport_delta new_size) override;

protected:
	// TODO: Replace with new renderer
	// presenter::LegacyDisplay *display;

public:
	coord::camhud bottomleft;
	coord::camhud topright;
	coord::camhud charsize;

	std::vector<util::col> termcolors;

	bool visible;

	Buf buf;
	renderer::Font font;

	// input::legacy::InputContext input_context;

	// the command state
	std::string command;
};

} // namespace console
} // namespace openage
