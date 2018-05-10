// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "qml_engine_with_singleton_items_info.h"

#include <algorithm>

namespace qtsdl {

QmlEngineWithSingletonItemsInfo::QmlEngineWithSingletonItemsInfo(std::vector<std::unique_ptr<GuiImageProviderImpl>> &&image_providers, GuiSingletonItemsInfo *singleton_items_info)
	:
	QmlEngineWithSingletonItemsInfo{image_providers, singleton_items_info} {
}

QmlEngineWithSingletonItemsInfo::QmlEngineWithSingletonItemsInfo(std::vector<std::unique_ptr<GuiImageProviderImpl>> &image_providers, GuiSingletonItemsInfo *singleton_items_info)
	:
	QQmlEngine{},
	image_providers(image_providers.size()),
	singleton_items_info{singleton_items_info} {

	std::transform(std::begin(image_providers), std::end(image_providers), std::begin(this->image_providers), [] (const std::unique_ptr<GuiImageProviderImpl> &image_provider) {
		return image_provider.get();
	});

	std::for_each(std::begin(image_providers), std::end(image_providers), [this] (std::unique_ptr<GuiImageProviderImpl> &image_provider) {
		auto id = image_provider->get_id();
		this->addImageProvider(id, image_provider.release());
	});
}

QmlEngineWithSingletonItemsInfo::~QmlEngineWithSingletonItemsInfo() {
	std::for_each(std::begin(this->image_providers), std::end(this->image_providers), [this] (GuiImageProviderImpl *image_provider) {
		image_provider->give_up();
		this->removeImageProvider(image_provider->get_id());
	});
}

GuiSingletonItemsInfo* QmlEngineWithSingletonItemsInfo::get_singleton_items_info() const {
	return this->singleton_items_info;
}

std::vector<GuiImageProviderImpl*> QmlEngineWithSingletonItemsInfo::get_image_providers() const {
	return this->image_providers;
}

} // namespace qtsdl
