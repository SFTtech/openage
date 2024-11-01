// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "config.h"

#if WITH_NCURSES
	#include <deque>
	#include <memory>
	#include <string>
	#include <vector>

	#include "event/demo/gamestate.h"
	#include "time/time.h"
	#include "util/vector.h"


namespace openage::event::demo {


class Gui {
public:
	Gui();
	void init();
	void clear();
	void update_screen();

	const std::vector<PongEvent> &get_inputs(const std::shared_ptr<PongPlayer> &player);
	void get_display_size(const std::shared_ptr<PongState> &state,
	                      const time::time_t &now);

	void draw(const std::shared_ptr<PongState> &state, const time::time_t &now);
	void draw_ball(util::Vector2d ball, const char *str);

	void log(const std::string &msg);

private:
	std::vector<PongEvent> input_cache;
	std::deque<std::string> log_msgs;
};

} // namespace openage::event::demo

#endif
