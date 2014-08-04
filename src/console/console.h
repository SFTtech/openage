#ifndef _CONSOLE_CONSOLE_H_
#define _CONSOLE_CONSOLE_H_

#include <vector>

#include <SDL2/SDL.h>

#include "../coord/camhud.h"
#include "../util/color.h"
#include "../font.h"
#include "../gamedata/color.h"

namespace openage {
namespace console {

extern coord::camhud bottomleft;
extern coord::camhud topright;
extern coord::camhud charsize;

extern std::vector<util::col> termcolors;

//init functions
void init(const std::vector<gamedata::palette_color> &termcolors);
void destroy();

//callback functions
bool on_engine_tick();
bool draw_console();
bool handle_inputs(SDL_Event *e);
bool on_window_resize();

/**
 * prints the given text on the console
 */
void write(const char *text);

} //namespace console
} //namespace openage

#endif //_CONSOLE_CONSOLE_H_
