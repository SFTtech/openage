#include "init.h"

#include <math.h>

#include "callbacks.h"
#include "../callbacks.h"
#include "console.h"
#include "../log.h"

namespace engine {
namespace console {

void init() {
        engine::callbacks::on_input.push_back(console::handle_inputs);
        engine::callbacks::on_engine_tick.push_back(console::on_engine_tick);
        engine::callbacks::on_drawhud.push_back(console::draw);
	engine::callbacks::on_resize.push_back(console::on_window_resize);

	font = new Font("DejaVu Sans Mono", "Book", 12);
	//this better be representative for the width of all other characters
	charsize.x = ceilf(font->internal_font->Advance("W", 1));
	charsize.y = ceilf(font->internal_font->LineHeight());

	log::dbg("console font character size: %hdx%hd", charsize.x, charsize.y);
}

void destroy() {
        for (auto m : messages) {
                delete[] m;
        }

	delete font;
}

} //namespace console
} //namespace engine
