// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "game_logic_caller.h"

#include "gui_callback.h"

namespace qtsdl {

GameLogicCaller::GameLogicCaller()
	:
	QObject{} {
}

void GameLogicCaller::set_game_logic_callback(GuiCallback *game_logic_callback) {
	QObject::disconnect(this, &GameLogicCaller::in_game_logic_thread, 0, 0);
	QObject::disconnect(this, &GameLogicCaller::in_game_logic_thread_blocking, 0, 0);
	QObject::connect(this, &GameLogicCaller::in_game_logic_thread, game_logic_callback, &GuiCallback::process);
	QObject::connect(this, &GameLogicCaller::in_game_logic_thread_blocking, game_logic_callback, &GuiCallback::process_blocking, Qt::DirectConnection);
}

} // namespace qtsdl
