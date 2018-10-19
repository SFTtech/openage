// Copyright 2016-2018 the openage authors. See copying.md for legal info.

#include "gui.h"

// the gui requires ncurses.
#if WITH_NCURSES

#include <algorithm>
#include <cstdio>
#include <ncurses.h>
#include <vector>


namespace openage::event::demo {

const std::vector<PongEvent> &Gui::get_inputs(const std::shared_ptr<PongPlayer> &player) {
	this->input_cache.clear();

	PongEvent evnt;
	evnt.player = player->id();
	evnt.state = PongEvent::IDLE;

	timeout(0);

	std::vector<int> inputs;
	int c;
	while ((c = getch()) != ERR) {
		if (std::find(inputs.begin(), inputs.end(), c) == inputs.end()) {
			inputs.push_back(c);
		}
	}

	for (auto input : inputs) {
		switch (input) {
		case KEY_DOWN:
			evnt.state = PongEvent::DOWN;
			mvprintw(1, 40, "DOWN");
			break;

		case KEY_UP:
			evnt.state = PongEvent::UP;
			mvprintw(1, 40, "UP");
			break;

		case 27:  // esc or alt
			erase();
			refresh();
			endwin();
			exit(0);
			break;

		case 'r':
		case ' ':
			evnt.state = PongEvent::START;
			break;

		default:
			evnt.state = PongEvent::IDLE;
			break;
		}

		this->input_cache.push_back(evnt);
	}

	if (this->input_cache.empty()) {
		// store 'idle' input to cancel movement
		this->input_cache.push_back(evnt);
	}

	return this->input_cache;
}


enum {
	COLOR_PLAYER1 = 1,
	COLOR_PLAYER2 = 2,
	COLOR_BALL    = 3,
	COLOR_DEBUG   = 4,

	COLOR_0 = 5,
	COLOR_1 = 6,
	COLOR_2 = 7,
	COLOR_3 = 8,
	COLOR_4 = 9,
};


constexpr const int max_log_msgs = 10;

Gui::Gui() {}


void Gui::init() {
	initscr();
	start_color();

	init_pair(COLOR_PLAYER1, COLOR_BLUE, COLOR_BLUE);
	init_pair(COLOR_PLAYER2, COLOR_RED, COLOR_RED);
	init_pair(COLOR_BALL, COLOR_BLUE, COLOR_WHITE);
	init_pair(COLOR_DEBUG, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLOR_0, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_1, COLOR_GREEN, COLOR_BLACK);

	keypad(stdscr, true);
	noecho();
	cbreak();
	curs_set(0);
	wnoutrefresh(curscr);

	int x, y;
	getmaxyx(stdscr, y, x);

	attron(COLOR_PAIR(COLOR_DEBUG));

	std::vector<const char*> buffer{
		"oooooooooo                                   ",
		" 888    888  ooooooo    ooooooo    oooooooo8 ",
		" 888oooo88 888     888 888   888  888    88o ",
		" 888       888     888 888   888   888oo888o ",
		"o888o        88ooo88  o888o o888o     88 888 ",
		"                                    888ooo888",
	};

	size_t colwidth = 0;
	for (const auto &c : buffer) {
		colwidth = std::max(colwidth, strlen(c));
	}

	int row = (y - buffer.size()) / 2;;
	int col = (x - colwidth) / 2;
	for (const auto &c : buffer) {
		mvprintw(row++, col, c);
	}

	attroff(COLOR_PAIR(COLOR_DEBUG));

	refresh();

	// wait for a keypress to begin game
	getch();
}


void Gui::clear() {
	erase();
}


void Gui::get_display_size(const std::shared_ptr<PongState> &state,
                           const curve::time_t &/*now*/) {
	// record the screen dimensions in the game state

	// TODO: make the display_boundary a curve as well.
	getmaxyx(stdscr, state->display_boundary[1], state->display_boundary[0]);
	state->display_boundary[1] -= 1;
}


void Gui::draw(const std::shared_ptr<PongState> &state, const curve::time_t &now) {
	attron(COLOR_PAIR(COLOR_DEBUG));

	// print the score
	attron(COLOR_PAIR(COLOR_DEBUG));
	mvprintw(2,
	         state->display_boundary[0] / 2 - 5,
	         "P1 %i | P2 %i",
	         state->p1->lives->get(now),
	         state->p2->lives->get(now));

	// draw the middle line
	mvvline(0, state->display_boundary[0] / 2, ACS_VLINE, state->display_boundary[1]);

	// debug information
	mvprintw(0, 1, "NOW:  %f", now.to_double());
	mvprintw(1, 1, "SCR:  %f | %f", state->display_boundary[0], state->display_boundary[1]);
	mvprintw(2, 1,
	         "P1:   %f, %f, %i",
	         state->p1->position->get(now),
	         state->p1->paddle_x,
	         state->p1->state->get(now).state);
	mvprintw(3, 1,
	         "P2:   %f, %f, %i",
	         state->p2->position->get(now),
	         state->p2->paddle_x,
	         state->p2->state->get(now).state);

	// ball position predictions, 10s into the future
	for (int i = 0; i < 10; i++) {
		auto i_as_ctt = curve::time_t::from_int(i);
		mvprintw((5 + i), 1,
		         "BALL in %03f: %f | %f; SPEED: %f | %f | PLpos: %f, PRpos: %f",
		         i_as_ctt.to_double(),
		         state->ball->position->get(now + i_as_ctt)[0],
		         state->ball->position->get(now + i_as_ctt)[1],
		         state->ball->speed->get(now + i_as_ctt)[0],
		         state->ball->speed->get(now + i_as_ctt)[1],
		         state->p1->position->get(now + i_as_ctt),
		         state->p2->position->get(now + i_as_ctt)
		);
	}

	// show log
	int msg_i = 0;
	for (auto & msg : this->log_msgs) {
		mvprintw((6 + msg_i), state->display_boundary[0]/2 + 10, msg.c_str());
		msg_i += 1;
	}


	// exit hint message
	mvprintw(state->display_boundary[1] - 1, 1, "Press ESC to exit");
	attroff(COLOR_PAIR(COLOR_DEBUG));

	// draw player 1 paddle
	attron(COLOR_PAIR(COLOR_PLAYER1));
	for (int i = -state->p1->size->get(now) / 2; i < state->p1->size->get(now) / 2; i++) {
		mvprintw(state->p1->position->get(now) + i, state->p1->paddle_x, "|");
	}
	attroff(COLOR_PAIR(COLOR_PLAYER1));

	// draw player 2 paddle
	attron(COLOR_PAIR(COLOR_PLAYER2));
	for (int i = -state->p2->size->get(now) / 2; i < state->p2->size->get(now) / 2; i++) {
		mvprintw(state->p2->position->get(now) + i, state->p2->paddle_x, "|");
	}
	attroff(COLOR_PAIR(COLOR_PLAYER2));

	// ball position prediction 10s into the future
	attron(COLOR_PAIR(COLOR_1));
	for (int i = 1; i < 100; ++i) {
		auto i_as_ctt = curve::time_t::from_double(i/10.0);
		draw_ball(state->ball->position->get(now + i_as_ctt), "x");
	}

	// draw the ball
	attron(COLOR_PAIR(COLOR_BALL));
	this->draw_ball(state->ball->position->get(now), "M"); // TODO: use "⚽"
	attroff(COLOR_PAIR(COLOR_BALL));
}


void Gui::update_screen() {
	// actually show screen output
	refresh();
}


void Gui::draw_ball(util::Vector2d pos, const char *str) {
	mvprintw((int)(pos[1]), (int)(pos[0]), "%s", str);
	standend();
}


void Gui::log(const std::string &msg) {
	if (this->log_msgs.size() >= max_log_msgs) {
		this->log_msgs.pop_back();
	}
	this->log_msgs.push_front(msg);
}

} // openage::event::demo

#endif
