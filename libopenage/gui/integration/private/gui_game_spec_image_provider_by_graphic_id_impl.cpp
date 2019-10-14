// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "gui_game_spec_image_provider_by_graphic_id_impl.h"

#include "../../../error/error.h"

#include "../../../gamestate/game_spec.h"

namespace openage::gui {

GuiGameSpecImageProviderByGraphicIdImpl::GuiGameSpecImageProviderByGraphicIdImpl(qtsdl::GuiEventQueue *render_updater)
	:
	GuiGameSpecImageProviderByIdImpl{render_updater} {
}

GuiGameSpecImageProviderByGraphicIdImpl::~GuiGameSpecImageProviderByGraphicIdImpl() = default;

const char* GuiGameSpecImageProviderByGraphicIdImpl::id() {
	return "by-graphic-id";
}

const char* GuiGameSpecImageProviderByGraphicIdImpl::get_id() const {
	return GuiGameSpecImageProviderByGraphicIdImpl::id();
}

openage::Texture* GuiGameSpecImageProviderByGraphicIdImpl::get_texture(int texture_id) {
	ENSURE(this->loaded_game_spec, "trying to actually get a texture from a non-loaded spec");
	return this->loaded_game_spec->get_texture(texture_id);
}

} // namespace openage::gui
