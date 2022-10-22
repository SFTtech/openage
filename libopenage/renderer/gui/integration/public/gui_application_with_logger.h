// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#pragma once

#include "../../guisys/public/gui_application.h"

namespace openage::renderer::gui {

/**
 * Houses gui logic event queue and attaches to game logger.
 */
class GuiApplicationWithLogger : public qtgui::GuiApplication {
public:
	GuiApplicationWithLogger();
	~GuiApplicationWithLogger();
};

} // namespace openage::renderer::gui
