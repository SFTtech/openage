// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

#include <QObject>

namespace qtsdl {

class GuiCallback;

/**
 * Attaches to the GuiCallbackImpl.
 */
class GameLogicCaller : public QObject {
	Q_OBJECT

public:
	explicit GameLogicCaller();

	/**
	 * Set up signal to be able to run code in the game logic thread.
	 */
	void set_game_logic_callback(GuiCallback *game_logic_callback);

signals:
	void in_game_logic_thread(const std::function<void()>& f) const;
	void in_game_logic_thread_blocking(const std::function<void()>& f) const;
};

} // namespace qtsdl
