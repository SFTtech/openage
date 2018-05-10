// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "main_args_link.h"

#include <QtQml>

#include "../error/error.h"

#include "engine_info.h"
#include "guisys/link/qml_engine_with_singleton_items_info.h"
#include "guisys/link/qtsdl_checked_static_cast.h"

namespace openage {
namespace gui {

namespace {
// register "MainArgs" in the qml engine to be used globally.
const int registration = qmlRegisterSingletonType<MainArgsLink>("yay.sfttech.openage", 1, 0, "MainArgs", &MainArgsLink::provider);
}


MainArgsLink::MainArgsLink(QObject *parent, const util::Path &asset_dir)
	:
	QObject{parent},
	asset_dir{asset_dir} {
	Q_UNUSED(registration);
}


QObject* MainArgsLink::provider(QQmlEngine *engine, QJSEngine*) {
	qtsdl::QmlEngineWithSingletonItemsInfo *engine_with_singleton_items_info = qtsdl::checked_static_cast<qtsdl::QmlEngineWithSingletonItemsInfo*>(engine);
	auto info = static_cast<EngineQMLInfo*>(engine_with_singleton_items_info->get_singleton_items_info());
	ENSURE(info, "globals were lost or not passed to the gui subsystem");

	// owned by the QML engine
	return new MainArgsLink{nullptr, info->asset_dir};
}

}} // namespace openage::gui
