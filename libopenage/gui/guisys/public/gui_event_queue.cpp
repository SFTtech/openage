// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "../public/gui_event_queue.h"

#include "../private/gui_event_queue_impl.h"

namespace qtsdl {

GuiEventQueue::GuiEventQueue()
	:
	impl{std::make_unique<GuiEventQueueImpl>()} {
}

GuiEventQueue::~GuiEventQueue() {
}

void GuiEventQueue::process_callbacks() {
	this->impl->process_callbacks();
}

} // namespace qtsdl
