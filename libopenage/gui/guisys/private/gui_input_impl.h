// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <atomic>

#include <QObject>

#include <SDL2/SDL_events.h>

namespace qtsdl {

class GuiRenderer;
class GuiEventQueue;
class GuiEventQueueImpl;

class GuiInputImpl : public QObject {
	Q_OBJECT

public:
	explicit GuiInputImpl(GuiRenderer *renderer, GuiEventQueue *game_logic_updater);
	virtual ~GuiInputImpl();

	/**
	 * Returns true if the event was accepted.
	 */
	bool process(SDL_Event *e);

signals:
	void input_event(std::atomic<bool> *processed, QEvent *ev, bool only_if_grabbed=false);

private:
	bool relay_input_event(QEvent *ev, bool only_if_grabbed=false);

	Qt::MouseButtons mouse_buttons_state;
	GuiEventQueueImpl *game_logic_updater;
};

} // namespace qtsdl
