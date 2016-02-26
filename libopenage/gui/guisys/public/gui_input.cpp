// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "../public/gui_input.h"

#include "../private/gui_input_impl.h"

namespace qtsdl {

GuiInput:: GuiInput(GuiRenderer *renderer, GuiEventQueue *game_logic_updater)
	:
	impl{std::make_unique<GuiInputImpl>(renderer, game_logic_updater)} {
}

GuiInput::~GuiInput() {
}

bool GuiInput::process(SDL_Event *event) {
	return this->impl->process(event);
}

} // namespace qtsdl
