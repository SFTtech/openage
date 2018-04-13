// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_event_queue_impl.h"

#include <cassert>

#ifdef __APPLE__
#include <QCoreApplication>
#endif
#include <QThread>

#include "../public/gui_event_queue.h"

namespace qtsdl {

GuiEventQueueImpl::GuiEventQueueImpl()
	:
	thread{QThread::currentThread()} {
}

GuiEventQueueImpl::~GuiEventQueueImpl() {
}

GuiEventQueueImpl* GuiEventQueueImpl::impl(GuiEventQueue *event_queue) {
	return event_queue->impl.get();
}

void GuiEventQueueImpl::process_callbacks() {
	assert(QThread::currentThread() == this->thread);
#ifdef __APPLE__
	if (QThread::currentThread() != QCoreApplication::instance()->thread()) this->callback_processor.processEvents();
#else
	this->callback_processor.processEvents();
#endif
}

QThread* GuiEventQueueImpl::get_thread() {
	return this->thread;
}

} // namespace qtsdl
