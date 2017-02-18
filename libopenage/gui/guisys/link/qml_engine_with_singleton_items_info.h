// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QQmlEngine>

#include "../private/gui_image_provider_impl.h"

namespace qtsdl {

struct GuiSingletonItemsInfo;

/**
 * The Qml Engine used by openage.
 *
 * It's extended to contain the "singleton items info" and a list of image providers.
 * The singleton item info is just a struct that allows to carry some variables
 * in the qml-engine, namely the openage-engine.
 *
 * That way, the openage-engine and the qml-engine have a 1:1 relation and
 * qml can access the main engine directly.
 */
class QmlEngineWithSingletonItemsInfo : public QQmlEngine {
	Q_OBJECT

public:
	explicit QmlEngineWithSingletonItemsInfo(std::vector<std::unique_ptr<GuiImageProviderImpl>> &&image_providers, GuiSingletonItemsInfo *singleton_items_info=nullptr);
	explicit QmlEngineWithSingletonItemsInfo(std::vector<std::unique_ptr<GuiImageProviderImpl>> &image_providers, GuiSingletonItemsInfo *singleton_items_info=nullptr);
	virtual ~QmlEngineWithSingletonItemsInfo();

	GuiSingletonItemsInfo* get_singleton_items_info() const;
	std::vector<GuiImageProviderImpl*> get_image_providers() const;

signals:
	void reload_gui_manually();

private:
	std::vector<GuiImageProviderImpl*> image_providers;
	GuiSingletonItemsInfo *singleton_items_info;
};

} // namespace qtsdl
