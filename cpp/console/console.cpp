// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#include "console.h"

#include "draw.h"
#include "../callbacks.h"
#include "../log.h"
#include "../util/error.h"
#include "../util/strings.h"

namespace openage {
namespace console {

/*
 * TODO:
 * multiple terminals on screen
 * resizable terminals
 * scrollbars
 * console input
 * log console, command console
 */

Console::Console(std::vector<gamedata::palette_color> &colortable)
	:
	bottomleft{0, 0},
	topright{1, 1},
	charsize{1, 1},
	visible(false),
	buf{{80, 25}, 1337, 80},
	font{"DejaVu Sans Mono", "Book", 12}
{
	termcolors.reserve(256);

	for (auto &c : colortable) {
		this->termcolors.emplace_back(c);
	}

	if (termcolors.size() != 256) {
		throw util::Error("Exactly 256 terminal colors are required.");
	}

	// this better be representative for the width of all other characters
	charsize.x = ceilf(font.internal_font->Advance("W", 1));
	charsize.y = ceilf(font.internal_font->LineHeight());

	log::dbg("console font character size: %hdx%hd", charsize.x, charsize.y);
}

Console::~Console () {}

void Console::register_to_engine(Game *game) {
	game->register_input_action(this);
	game->register_tick_action(this);
	game->register_drawhud_action(this);
	game->register_resize_action(this);
}

void Console::write(const char *text) {
	this->buf.write(text);
	this->buf.write('\n');
}

bool Console::on_tick() {
	if (!this->visible) {
		return true;
	}

	// TODO: handle stuff such as cursor blinking,
	// repeating held-down keys
	return true;
}

bool Console::on_drawhud() {
	if (!this->visible) {
		return true;
	}

	draw::to_opengl(this);

	return true;
}

bool Console::on_input(SDL_Event *e) {
	if ((e->type == SDL_KEYDOWN) && (((SDL_KeyboardEvent *) e)->keysym.sym == SDLK_BACKQUOTE)) {
		visible = !visible;
	}

	// only handle inputs if the console is visible
	if (!this->visible) {
		return true;
	}

	switch (e->type) {
	case SDL_KEYDOWN:
		//TODO handle key inputs

		//do not allow anyone else to handle this input
		return false;
		break;
	}

	return true;
}

bool Console::on_resize(coord::window new_size) {
	coord::pixel_t w = this->buf.dims.x * this->charsize.x;
	coord::pixel_t h = this->buf.dims.y * this->charsize.y;

	this->bottomleft = {(new_size.x - w) / 2, (new_size.y - h) / 2};
	this->topright = {this->bottomleft.x + w, this->bottomleft.y - h};

	return true;
}

} //namespace console
} //namespace openage
