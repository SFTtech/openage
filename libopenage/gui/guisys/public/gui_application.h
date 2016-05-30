// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

namespace qtsdl {

class GuiApplicationImpl;

/**
 * Houses gui logic event queue.
 */
class GuiApplication {
public:
	GuiApplication();
	GuiApplication(std::shared_ptr<GuiApplicationImpl> application);
	~GuiApplication();

	void processEvents();

private:
	std::shared_ptr<GuiApplicationImpl> application;
};

} // namespace qtsdl
