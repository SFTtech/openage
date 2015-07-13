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

namespace openage {
namespace console {

class Console : InputHandler, TickHandler, HudHandler, ResizeHandler {

public:
	Console();
	~Console();

	coord::camhud bottomleft;
	coord::camhud topright;
	coord::camhud charsize;

	std::vector<util::col> termcolors;

	bool visible;

	Buf buf;
	Font font;

	input::InputContext input_context;

	//callback functions
	bool on_engine_tick();
	bool draw_console();
	bool handle_inputs(SDL_Event *e);
	bool on_window_resize();

	void load_colors(std::vector<gamedata::palette_color> &colortable);

	/**
	 * register this console to the engine.
	 * this leads to the drawing calls, and input handling.
	 */
	void register_to_engine(Engine *engine);

	/**
	 * prints the given text on the console.
	 */
	void write(const char *text);

	virtual bool on_drawhud();
	virtual bool on_tick();
	virtual bool on_input(SDL_Event *event);
	virtual bool on_resize(coord::window new_size);
};

} //namespace console
} //namespace openage

#endif
