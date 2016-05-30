// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QQmlEngine>

#include "../private/gui_image_provider_impl.h"

namespace qtsdl {

struct GuiSingletonItemsInfo;

class QmlEngineWithSingletonItemsInfo : public QQmlEngine {
	Q_OBJECT

public:
	explicit QmlEngineWithSingletonItemsInfo(std::vector<std::unique_ptr<GuiImageProviderImpl>> &&image_providers, GuiSingletonItemsInfo *singleton_items_info=nullptr);
	explicit QmlEngineWithSingletonItemsInfo(std::vector<std::unique_ptr<GuiImageProviderImpl>> &image_providers, GuiSingletonItemsInfo *singleton_items_info=nullptr);
	virtual ~QmlEngineWithSingletonItemsInfo();

	GuiSingletonItemsInfo* get_singleton_items_info() const;
	std::vector<GuiImageProviderImpl*> get_image_providers() const;

private:
	std::vector<GuiImageProviderImpl*> image_providers;
	GuiSingletonItemsInfo *singleton_items_info;
};

} // namespace qtsdl
