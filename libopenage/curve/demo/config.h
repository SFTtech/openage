// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

// Define this to draw an ncurses based demo.
// Without the GUI flag, just a trace of event is printed out
#define GUI

// If this is defined, player 1 can be played with the arrow keys.
// else the player is replaced by an AI.
//#define HUMAN

// This can take the following values:
// 0: run in real time with real deltas
// 1: run very slow
// 2: run very fast
#define REALTIME 1
