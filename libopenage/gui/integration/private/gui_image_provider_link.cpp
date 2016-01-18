// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_image_provider_link.h"

#include <algorithm>

#include <QtQml>

#include "../../../error/error.h"

#include "../../guisys/link/qml_engine_with_singleton_items_info.h"
#include "../../guisys/link/qtsdl_checked_static_cast.h"

#include "../../game_spec_link.h"
#include "gui_game_spec_image_provider_impl.h"

#include "gui_game_spec_image_provider_by_filename_impl.h"
#include "gui_game_spec_image_provider_by_graphic_id_impl.h"
#include "gui_game_spec_image_provider_by_terrain_id_impl.h"

namespace openage {
namespace gui {

namespace {
const int registration_by_filename = qmlRegisterSingletonType<GuiImageProviderLink>("yay.sfttech.openage", 1, 0, "ImageProviderByFilename", &GuiImageProviderLink::provider_by_filename);
const int registration_by_id = qmlRegisterSingletonType<GuiImageProviderLink>("yay.sfttech.openage", 1, 0, "ImageProviderById", &GuiImageProviderLink::provider_by_graphic_id);
const int registration_by_terrain_id = qmlRegisterSingletonType<GuiImageProviderLink>("yay.sfttech.openage", 1, 0, "ImageProviderByTerrainId", &GuiImageProviderLink::provider_by_terrain_id);
}

GuiImageProviderLink::GuiImageProviderLink(QObject *parent, GuiGameSpecImageProviderImpl &image_provider)
	:
	QObject{parent},
	image_provider{image_provider},
	game_spec{} {
	Q_UNUSED(registration_by_filename);
	Q_UNUSED(registration_by_id);
	Q_UNUSED(registration_by_terrain_id);
}

GuiImageProviderLink::~GuiImageProviderLink() {
}

GameSpecLink* GuiImageProviderLink::get_game_spec() const {
	return this->game_spec;
}

void GuiImageProviderLink::set_game_spec(GameSpecLink *game_spec) {
	if (this->game_spec != game_spec) {
		if (this->game_spec)
			QObject::disconnect(this->game_spec.data(), &GameSpecLink::game_spec_loaded, this, &GuiImageProviderLink::on_game_spec_loaded);

		if (!game_spec)
			this->image_provider.on_game_spec_invalidated();

		this->game_spec = game_spec;

		if (this->game_spec) {
			QObject::connect(this->game_spec.data(), &GameSpecLink::game_spec_loaded, this, &GuiImageProviderLink::on_game_spec_loaded);

			if (std::shared_ptr<GameSpec> spec = this->game_spec->get_loaded_spec())
				this->image_provider.on_game_spec_loaded(spec);
		}
	}
}

QObject* GuiImageProviderLink::provider(QQmlEngine *engine, const char *id) {
	qtsdl::QmlEngineWithSingletonItemsInfo *engine_with_singleton_items_info = qtsdl::checked_static_cast<qtsdl::QmlEngineWithSingletonItemsInfo*>(engine);
	auto image_providers = engine_with_singleton_items_info->get_image_providers();

	auto found_it = std::find_if(std::begin(image_providers), std::end(image_providers), [id] (qtsdl::GuiImageProviderImpl *image_provider) {
		return image_provider->get_id() == id;
	});

	ENSURE(found_it != std::end(image_providers), "The image provider '" << id << "' wasn't created or wasn't passed to the QML engine creation function.");

	// owned by the QML engine
	return new GuiImageProviderLink{nullptr, *qtsdl::checked_static_cast<GuiGameSpecImageProviderImpl*>(*found_it)};
}

QObject* GuiImageProviderLink::provider_by_filename(QQmlEngine *engine, QJSEngine*) {
	return GuiImageProviderLink::provider(engine, GuiGameSpecImageProviderByFilenameImpl::id());
}

QObject* GuiImageProviderLink::provider_by_graphic_id(QQmlEngine *engine, QJSEngine*) {
	return GuiImageProviderLink::provider(engine, GuiGameSpecImageProviderByGraphicIdImpl::id());
}

QObject* GuiImageProviderLink::provider_by_terrain_id(QQmlEngine *engine, QJSEngine*) {
	return GuiImageProviderLink::provider(engine, GuiGameSpecImageProviderByTerrainIdImpl::id());
}

void GuiImageProviderLink::on_game_spec_loaded(GameSpecLink *game_spec, std::shared_ptr<GameSpec> loaded_game_spec) {
	if (this->game_spec == game_spec)
		this->image_provider.on_game_spec_loaded(loaded_game_spec);
}

}} // namespace openage::gui
