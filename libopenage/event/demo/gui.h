// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "gamestate.h"


namespace openage::event::demo {

#if WITH_NCURSES

class Gui {
public:
	Gui();
	void init();
	void clear();
	void update_screen();

	const std::vector<PongEvent> &get_inputs(const std::shared_ptr<PongPlayer> &player);

	void draw(std::shared_ptr<PongState> &state, const curve::time_t &now);
	void draw_ball(util::Vector2d ball, const char *str);

private:
	std::vector<PongEvent> input_cache;
};

#endif

} // openage::event::demo
