// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "gui_image_provider_link.h"

#include <algorithm>

#include <QtQml>

#include "../../../error/error.h"

#include "../../guisys/link/qml_engine_with_singleton_items_info.h"
#include "../../guisys/link/qtsdl_checked_static_cast.h"

namespace openage::gui {

GuiImageProviderLink::GuiImageProviderLink(QObject *parent) :
	QObject{parent} {
}

GuiImageProviderLink::~GuiImageProviderLink() = default;

QObject *GuiImageProviderLink::provider(QQmlEngine *engine, const char *id) {
	qtsdl::QmlEngineWithSingletonItemsInfo *engine_with_singleton_items_info = qtsdl::checked_static_cast<qtsdl::QmlEngineWithSingletonItemsInfo *>(engine);
	auto image_providers = engine_with_singleton_items_info->get_image_providers();

	auto found_it = std::find_if(std::begin(image_providers), std::end(image_providers), [id](qtsdl::GuiImageProviderImpl *image_provider) {
		return image_provider->get_id() == id;
	});

	ENSURE(found_it != std::end(image_providers), "The image provider '" << id << "' wasn't created or wasn't passed to the QML engine creation function.");

	// owned by the QML engine
	return nullptr;
}

} // namespace openage::gui
