// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "console.h"

#include "../log/log.h"
#include "../error/error.h"
#include "../callbacks.h"
#include "../util/strings.h"

#include "draw.h"

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

Console::Console()
	:
	bottomleft{0, 0},
	topright{1, 1},
	charsize{1, 1},
	visible(false),
	buf{{80, 25}, 1337, 80},
	font{"DejaVu Sans Mono", "Book", 12}
{
	termcolors.reserve(256);

	// this better be representative for the width of all other characters
	charsize.x = ceilf(font.internal_font->Advance("W", 1));
	charsize.y = ceilf(font.internal_font->LineHeight());

	log::log(MSG(dbg) << "Console font character size: " << charsize.x << "x" << charsize.y);

	// Adjust the corners of the console based on the default buffer size and char size
	topright.x = charsize.x * buf.dims.x;
	topright.y = charsize.y * buf.dims.y;
}

Console::~Console () {}

void Console::load_colors(std::vector<gamedata::palette_color> &colortable) {
	for (auto &c : colortable) {
		this->termcolors.emplace_back(c);
	}

	if (termcolors.size() != 256) {
		throw Error(MSG(err) << "Exactly 256 terminal colors are required.");
	}
}

void Console::register_to_engine(Engine *engine) {
	engine->register_input_action(this);
	engine->register_tick_action(this);
	engine->register_drawhud_action(this);
	engine->register_resize_action(this);

	// TODO bind any needed input to InputContext

	// Bind the console toggle key globally
	auto &input = engine->get_input_manager();
	input.get_global_context()
	        .bind(input::action_t::TOGGLE_CONSOLE, [this, &input](const input::action_arg_t &) {
		if (!visible) { // Show the console, add keybinds
			visible = true;
			input.override_context(&this->input_context);
		} else { // Hide the console, remove the keybinds
			visible = false;
			input.remove_context();
		}
	});
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
