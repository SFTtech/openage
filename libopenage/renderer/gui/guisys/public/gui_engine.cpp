// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#include "renderer/gui/guisys/public/gui_engine.h"

#include "renderer/gui/guisys/private/gui_engine_impl.h"

namespace qtgui {

GuiQmlEngine::GuiQmlEngine(std::shared_ptr<GuiRenderer> renderer) :
	impl{std::make_unique<GuiQmlEngineImpl>(renderer)} {
}

GuiQmlEngine::~GuiQmlEngine() = default;

} // namespace qtgui
