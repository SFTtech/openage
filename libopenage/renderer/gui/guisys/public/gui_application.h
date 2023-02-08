// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

namespace qtgui {

class GuiApplicationImpl;

/**
 * Houses gui logic event queue.
 */
class GuiApplication {
public:
	GuiApplication();
	GuiApplication(std::shared_ptr<GuiApplicationImpl> application);
	~GuiApplication();

	void process_events();

private:
	std::shared_ptr<GuiApplicationImpl> application;
};

} // namespace qtgui
