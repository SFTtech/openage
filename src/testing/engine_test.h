#ifndef _OPENAGE_INIT_H_
#define _OPENAGE_INIT_H_


#include <SDL2/SDL.h>
#include "../util/dir.h"

namespace testing {

/**
 * run the main testing routine for the game.
 *
 * this invokes the main game for now.
 */
int run_game(openage::Arguments *args);

void gametest_init(openage::util::Dir &data_dir);
void gametest_destroy();

bool on_engine_tick();
bool draw_method();
bool hud_draw_method();
bool input_handler(SDL_Event *e);

} //namespace testing

#endif //_OPENAGE_INIT_H_
