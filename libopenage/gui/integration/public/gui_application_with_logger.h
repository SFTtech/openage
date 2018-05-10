// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../../guisys/public/gui_application.h"

namespace openage {
namespace gui {

/**
 * Houses gui logic event queue and attaches to game logger.
 */
class GuiApplicationWithLogger : public qtsdl::GuiApplication {
public:
	GuiApplicationWithLogger();
	~GuiApplicationWithLogger();
};

}} // namespace openage::gui
