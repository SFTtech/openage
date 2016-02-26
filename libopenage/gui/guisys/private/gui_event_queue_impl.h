// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QtGlobal>
#include <QEventLoop>

QT_FORWARD_DECLARE_CLASS(QThread)

namespace qtsdl {

class GuiEventQueue;

/**
 * Provides synchronization with some game thread.
 */
class GuiEventQueueImpl {
public:
	explicit GuiEventQueueImpl();
	~GuiEventQueueImpl();

	static GuiEventQueueImpl* impl(GuiEventQueue *event_queue);

	void process_callbacks();

	QThread* get_thread();

private:
	QThread * const thread;
	QEventLoop callback_processor;
};

} // namespace qtsdl
