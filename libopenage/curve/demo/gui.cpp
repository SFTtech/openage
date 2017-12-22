// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "gui.h"

#include <ncurses.h>
#include <stdio.h>

namespace openage {
namespace curvepong {

std::vector<event> &Gui::getInputs(const std::shared_ptr<PongPlayer> &player) {
	input_cache.clear();
	event evnt;
	evnt.player = player->id();
	evnt.state  = event::IDLE;
	timeout(0);
	int c = getch();
	mvprintw(0,30, "IN: %i", c);
	switch (c) {
	case KEY_DOWN:
		evnt.state = event::DOWN;
		input_cache.push_back(evnt);
		mvprintw(1, 1, "DOWN");
		break;
	case KEY_UP:
		evnt.state = event::UP;
		input_cache.push_back(evnt);
		mvprintw(1, 1, "UP");
		break;
	case ' ':
		evnt.state = event::START;
		break;
	case 27:  // esc or alt
		endwin();
		exit(0);
		break;
	case 114: //r
		evnt.state = event::START;
		input_cache.push_back(evnt);
		break;
	default: break;
	}

	return input_cache;
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


Gui::Gui() {
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
	curs_set(0);

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

	getch();
}


void Gui::draw(std::shared_ptr<State> &state, const curve::curve_time_t &now) {
	//	clear();
	// Print Score
	attron(COLOR_PAIR(COLOR_DEBUG));
	getmaxyx(stdscr, state->resolution[1], state->resolution[0]);
	state->resolution[1] -= 1;
	attron(COLOR_PAIR(COLOR_DEBUG));
	mvprintw(2,
	         state->resolution[0] / 2 - 5,
	         "P1 %i | P2 %i",
	         state->p1->lives->get(now),
	         state->p2->lives->get(now));

	mvvline(0, state->resolution[0] / 2, ACS_VLINE, state->resolution[1]);
	mvprintw(0, 1, "NOW:  %d", now);
	mvprintw(1, 1, "SCR:  %i | %i", state->resolution[0], state->resolution[1]);
	mvprintw(2,
	         1,
	         "P1:   %f, %f, %i",
	         state->p1->position->get(now),
	         state->p1->y,
	         state->p1->state->get(now).state);
	mvprintw(3,
	         1,
	         "P2:   %f, %f, %i",
	         state->p2->position->get(now),
	         state->p2->y,
	         state->p2->state->get(now).state);
	for (int i = 0; i < 100; i += 10) {
		mvprintw(4 + i / 10,
		         1,
		         "BALL in %03i: %f | %f; SPEED: %f | %f",
		         i,
		         state->ball->position->get(now + i)[0],
		         state->ball->position->get(now + i)[1],
		         state->ball->speed->get(now + i)[0],
		         state->ball->speed->get(now + i)[1]);
	}
	mvprintw(state->resolution[1] - 1, 1, "Press ESC twice to Exit");
	attroff(COLOR_PAIR(COLOR_DEBUG));

	attron(COLOR_PAIR(COLOR_PLAYER1));
	for (int i = -state->p1->size->get(now) / 2; i < state->p1->size->get(now) / 2; i++) {
		mvprintw(state->p1->position->get(now) + i, state->p1->y, "|");
	}
	attroff(COLOR_PAIR(COLOR_PLAYER1));

	attron(COLOR_PAIR(COLOR_PLAYER2));
	for (int i = -state->p2->size->get(now) / 2; i < state->p2->size->get(now) / 2; i++) {
		mvprintw(state->p2->position->get(now) + i, state->p2->y, "|");
	}
	attroff(COLOR_PAIR(COLOR_PLAYER2));

	attron(COLOR_PAIR(COLOR_1));
	for (int i = 1; i < 9999; ++i) {
		draw_ball(state->ball->position->get(now + i), 'X');
	}
	attron(COLOR_PAIR(COLOR_0));
	draw_ball(state->ball->position->get(now), 'M');
	/*attron(COLOR_PAIR(COLOR_BALL));
	  mvprintw(state->ball->position->get(now)[1],
	  state->ball->position->get(now)[0],
	  "o");
	*/
	attroff(COLOR_PAIR(COLOR_BALL));
	refresh();
	erase();
}


void Gui::draw_ball(util::Vector<2> pos, char chr) {
	mvprintw((int)(pos[1]), (int)(pos[0]), "%c", chr);
	standend();
}
}} // openage::curvepong
