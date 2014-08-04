#include "console.h"

#include "buf.h"
#include "draw.h"
#include "../callbacks.h"
#include "../engine.h"
#include "../font.h"
#include "../log.h"
#include "../util/error.h"
#include "../util/strings.h"

namespace openage {
namespace console {

bool visible = false;

coord::camhud bottomleft {0, 0};
coord::camhud topright {1, 1};

Buf *buf = nullptr;
Font *font = nullptr;
coord::camhud charsize {1, 1};

std::vector<util::col> termcolors;

void write(const char *text) {
	buf->write(text);
	buf->write('\n');
}

bool on_engine_tick() {
	if(!visible) {
		return true;
	}

	//TODO handle stuff such as cursor blinking,
	//repeating held-down keys (from engine::input::keystate)
	return true;
}

bool draw_console() {
	if(!visible) {
		return true;
	}

	draw::to_opengl(buf, font, bottomleft, charsize);

	return true;
}

bool handle_inputs(SDL_Event *e) {
	if ((e->type == SDL_KEYDOWN) && (((SDL_KeyboardEvent *) e)->keysym.sym == SDLK_BACKQUOTE)) {
		visible = !visible;
	}

	//only handle inputs if the console is visible
	if(!visible) {
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

bool on_window_resize() {
	coord::pixel_t w = buf->dims.x * charsize.x;
	coord::pixel_t h = buf->dims.y * charsize.y;

	console::bottomleft = {(window_size.x - w) / 2, (window_size.y - h) / 2};
	console::topright = {console::bottomleft.x + w, console::bottomleft.y - h};

	return true;
}



void init(const std::vector<gamedata::palette_color> &colortable) {
	termcolors.reserve(256);
	for (auto c : colortable) {
		termcolors.emplace_back(c);
	}
	if (termcolors.size() != 256) {
		throw util::Error("Exactly 256 terminal colors are required.");
	}

	callbacks::on_input.push_back(console::handle_inputs);
	callbacks::on_engine_tick.push_back(console::on_engine_tick);
	callbacks::on_drawhud.push_back(console::draw_console);
	callbacks::on_resize.push_back(console::on_window_resize);

	buf = new Buf({80, 25}, 1337, 80);
	font = new Font("DejaVu Sans Mono", "Book", 12);
	//this better be representative for the width of all other characters
	charsize.x = ceilf(font->internal_font->Advance("W", 1));
	charsize.y = ceilf(font->internal_font->LineHeight());

	log::dbg("console font character size: %hdx%hd", charsize.x, charsize.y);
}

void destroy() {
	delete buf;
	delete font;
}

} //namespace console
} //namespace openage
