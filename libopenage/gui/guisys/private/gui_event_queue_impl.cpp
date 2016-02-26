// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_event_queue_impl.h"

#include <cassert>

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
	this->callback_processor.processEvents();
}

QThread* GuiEventQueueImpl::get_thread() {
	return this->thread;
}

} // namespace qtsdl
