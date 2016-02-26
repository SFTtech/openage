// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

namespace qtsdl {

class GuiEventQueueImpl;

/**
 * Provides synchronization with some game thread.
 */
class GuiEventQueue {
public:
	explicit GuiEventQueue();
	~GuiEventQueue();

	void process_callbacks();

private:
	friend class GuiEventQueueImpl;
	std::unique_ptr<GuiEventQueueImpl> impl;
};

} // namespace qtsdl
