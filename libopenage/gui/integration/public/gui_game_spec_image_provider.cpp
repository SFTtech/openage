// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "gui_game_spec_image_provider.h"

#include "../../../error/error.h"

#include "../private/gui_game_spec_image_provider_by_filename_impl.h"
#include "../private/gui_game_spec_image_provider_by_graphic_id_impl.h"
#include "../private/gui_game_spec_image_provider_by_terrain_id_impl.h"

namespace openage {
namespace gui {

GuiGameSpecImageProvider::GuiGameSpecImageProvider(qtsdl::GuiEventQueue *render_updater, Type type)
	:
	GuiImageProvider{[render_updater, type] () -> std::unique_ptr<qtsdl::GuiImageProviderImpl> {
		switch(type) {
		case Type::ByFilename:
			return std::make_unique<GuiGameSpecImageProviderByFilenameImpl>(render_updater);

		case Type::ByGraphicId:
			return std::make_unique<GuiGameSpecImageProviderByGraphicIdImpl>(render_updater);

		case Type::ByTerrainId:
			return std::make_unique<GuiGameSpecImageProviderByTerrainIdImpl>(render_updater);

		default:
			break;
		}

		ENSURE(false, "unhandled image provider type");
		return std::unique_ptr<qtsdl::GuiImageProviderImpl>{};
	}()} {
}

GuiGameSpecImageProvider::~GuiGameSpecImageProvider() {
}

}} // namespace openage::gui
