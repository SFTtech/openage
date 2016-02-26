// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "main_args_link.h"

#include <QtQml>

#include "../error/error.h"

#include "../game_singletons_info.h"
#include "guisys/link/qml_engine_with_singleton_items_info.h"
#include "guisys/link/qtsdl_checked_static_cast.h"

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterSingletonType<MainArgsLink>("yay.sfttech.openage", 1, 0, "MainArgs", &MainArgsLink::provider);
}

MainArgsLink::MainArgsLink(QObject *parent, const QString &data_dir)
	:
	QObject{parent},
	data_dir{data_dir} {
	Q_UNUSED(registration);
}

MainArgsLink::~MainArgsLink() {
}

QString MainArgsLink::get_data_dir() const {
	return this->data_dir;
}

QObject* MainArgsLink::provider(QQmlEngine *engine, QJSEngine*) {
	qtsdl::QmlEngineWithSingletonItemsInfo *engine_with_singleton_items_info = qtsdl::checked_static_cast<qtsdl::QmlEngineWithSingletonItemsInfo*>(engine);
	auto info = static_cast<GameSingletonsInfo*>(engine_with_singleton_items_info->get_singleton_items_info());
	ENSURE(info, "globals were lost or not passed to the gui subsystem");

	// owned by the QML engine
	return new MainArgsLink{nullptr, QString::fromStdString(info->data_dir)};
}

}} // namespace openage::gui
