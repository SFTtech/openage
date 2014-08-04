#include "callbacks.h"

namespace openage {

namespace callbacks {
std::vector<tick_method_ptr> on_engine_tick;
std::vector<drawgame_method_ptr> on_drawgame;
std::vector<drawhud_method_ptr> on_drawhud;
std::vector<input_handler_ptr> on_input;
std::vector<resize_handler_ptr> on_resize;
}

} //namespace openage
