// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "gui_game_spec_image_provider_by_terrain_id_impl.h"

#include "../../../error/error.h"

#include "../../../gamestate/game_spec.h"

namespace openage::gui {

GuiGameSpecImageProviderByTerrainIdImpl::GuiGameSpecImageProviderByTerrainIdImpl(qtsdl::GuiEventQueue *render_updater)
	:
	GuiGameSpecImageProviderByIdImpl{render_updater} {
}

GuiGameSpecImageProviderByTerrainIdImpl::~GuiGameSpecImageProviderByTerrainIdImpl() = default;

const char* GuiGameSpecImageProviderByTerrainIdImpl::id() {
	return "by-terrain-id";
}

const char* GuiGameSpecImageProviderByTerrainIdImpl::get_id() const {
	return GuiGameSpecImageProviderByTerrainIdImpl::id();
}

openage::Texture* GuiGameSpecImageProviderByTerrainIdImpl::get_texture(int texture_id) {
	ENSURE(this->loaded_game_spec, "trying to actually get a texture from a non-loaded spec");
	auto meta = this->loaded_game_spec->get_terrain_meta();
	return meta && texture_id >=0 && texture_id < std::distance(std::begin(meta->textures), std::end(meta->textures)) ? meta->textures[texture_id] : nullptr;
}

} // namespace openage::gui
