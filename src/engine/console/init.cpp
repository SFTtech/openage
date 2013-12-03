#include "init.h"

#include "callbacks.h"
#include "../callbacks.h"
#include "console.h"

namespace engine {
namespace console {

void init() {
        engine::callbacks::on_input.push_back(console::handle_inputs);
        engine::callbacks::on_engine_tick.push_back(console::on_engine_tick);
        engine::callbacks::on_drawhud.push_back(console::draw);
	engine::callbacks::on_resize.push_back(console::on_window_resize);

	font = new Font("DejaVu Sans Mono", "Book", 12);
}

void destroy() {
        for (auto m : messages) {
                delete[] m;
        }

	delete font;
}

} //namespace console
} //namespace engine
