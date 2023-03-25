// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "gui_input.h"

#include "renderer/gui/guisys/private/gui_input_impl.h"

namespace qtgui {

GuiInput::GuiInput(const std::shared_ptr<GuiRenderer> &renderer) :
	impl{std::make_unique<GuiInputImpl>(renderer)} {
}


bool GuiInput::process(const std::shared_ptr<QEvent> &event) {
	return this->impl->process(event);
}

} // namespace qtgui
