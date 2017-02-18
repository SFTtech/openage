// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "gui.h"

#include <ncurses.h>
#include <stdio.h>

namespace openage {
namespace tubepong {

std::vector<event> &Gui::getInputs(const PongPlayer &player) {
	input_cache.clear();
	event evnt;
	evnt.player = player.id;
	evnt.state  = event::IDLE;
	timeout(0);
	int c = getch();
	// mvprintw(0,1, "IN: %i", c);
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
	case ' ': evnt.state = event::START; break;
	case 27:  // esc or alt
		endwin();
		exit(0);
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
	//	getmaxyx(stdscr,state.resolution[1], state.resolution[0]);

	attron(COLOR_PAIR(COLOR_DEBUG));
	mvprintw(
		4, 5, "          oooooooooo                                          ");
	mvprintw(
		5, 5, "          888    888  ooooooo    ooooooo    oooooooo8         ");
	mvprintw(
		6, 5, "          888oooo88 888     888 888   888  888    88o         ");
	mvprintw(
		7, 5, "          888       888     888 888   888   888oo888o         ");
	mvprintw(
		8, 5, "         o888o        88ooo88  o888o o888o     88 888         ");
	mvprintw(
		9, 5, "                                            888ooo888         ");
	attroff(COLOR_PAIR(COLOR_DEBUG));

	getch();
}

void Gui::draw(PongState &state, const tube::tube_time_t &now) {
	erase();
	//	clear();
	// Print Score
	attron(COLOR_PAIR(COLOR_DEBUG));
	getmaxyx(stdscr, state.resolution[1], state.resolution[0]);
	attron(COLOR_PAIR(COLOR_DEBUG));
	mvprintw(2,
	         state.resolution[0] / 2 - 5,
	         "P1 %i | P2 %i",
	         state.p1.lives(now),
	         state.p2.lives(now));

	mvvline(0, state.resolution[0] / 2, ACS_VLINE, state.resolution[1]);
	mvprintw(0, 1, "NOW:  %f", now);
	mvprintw(1, 1, "SCR:  %i | %i", state.resolution[0], state.resolution[1]);
	mvprintw(2,
	         1,
	         "P1:   %f, %f, %i",
	         state.p1.position(now),
	         state.p1.y,
	         state.p1.state(now).state);
	mvprintw(3,
	         1,
	         "P2:   %f, %f, %i",
	         state.p2.position(now),
	         state.p2.y,
	         state.p2.state(now).state);
	for (int i = 0; i < 1000; i += 100) {
		mvprintw(4 + i / 100,
		         1,
		         "BALL in %03i: %f | %f; SPEED: %f | %f",
		         i,
		         state.ball.position(now + i)[0],
		         state.ball.position(now + i)[1],
		         state.ball.speed(now + i)[0],
		         state.ball.speed(now + i)[1]);
	}
	mvprintw(state.resolution[1] - 1, 1, "Press ESC twice to Exit");
	attroff(COLOR_PAIR(COLOR_DEBUG));

	attron(COLOR_PAIR(COLOR_PLAYER1));
	for (int i = -state.p1.size(now) / 2; i < state.p1.size(now) / 2; i++) {
		mvprintw(state.p1.position(now) + i, state.p1.y, "|");
	}
	attroff(COLOR_PAIR(COLOR_PLAYER1));

	attron(COLOR_PAIR(COLOR_PLAYER2));
	for (int i = -state.p2.size(now) / 2; i < state.p2.size(now) / 2; i++) {
		mvprintw(state.p2.position(now) + i, state.p2.y, "|");
	}
	attroff(COLOR_PAIR(COLOR_PLAYER2));

	for (int i = 0; i < 9999; ++i) {
		draw_ball(state.ball.position(now + i * 50), i);
	}

	/*attron(COLOR_PAIR(COLOR_BALL));
	  mvprintw(state.ball.position(now)[1],
	  state.ball.position(now)[0],
	  "o");
	*/
	attroff(COLOR_PAIR(COLOR_BALL));
	refresh();
}

void Gui::draw_ball(util::Vector<2> pos, int idx) {
	switch (idx) {
	case 0: attron(COLOR_PAIR(COLOR_0)); break;
	default:
	case 1: attron(COLOR_PAIR(COLOR_1)); break;
	}

	mvprintw((int)(pos[1]), (int)(pos[0]), "X");
	standend();
}
}
}  // openage::tubepong
