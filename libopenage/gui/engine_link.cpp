// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "engine_link.h"

#include <QtQml>

#include "../error/error.h"

#include "../engine.h"
#include "../game_singletons_info.h"

#include "guisys/link/qml_engine_with_singleton_items_info.h"
#include "guisys/link/qtsdl_checked_static_cast.h"

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterSingletonType<EngineLink>("yay.sfttech.openage", 1, 0, "Engine", &EngineLink::provider);
}

EngineLink::EngineLink(QObject *parent, Engine *engine)
	:
	GuiSingletonItem{parent},
	core{engine} {
	Q_UNUSED(registration);

	ENSURE(!unwrap(this)->gui_link, "Sharing singletons between QML engines is not supported for now.");

	if (unwrap(this)->gui_link)
		qFatal("Sharing singletons between QML engines is not supported for now.");

	QObject::connect(&unwrap(this)->gui_signals, &EngineSignals::global_binds_changed, this, &EngineLink::on_global_binds_changed);
	unwrap(this)->announce_global_binds();
}

EngineLink::~EngineLink() {
	unwrap(this)->gui_link = nullptr;
}

QObject* EngineLink::provider(QQmlEngine *engine, QJSEngine*) {
	qtsdl::QmlEngineWithSingletonItemsInfo *engine_with_singleton_items_info = qtsdl::checked_static_cast<qtsdl::QmlEngineWithSingletonItemsInfo*>(engine);
	auto info = static_cast<GameSingletonsInfo*>(engine_with_singleton_items_info->get_singleton_items_info());
	ENSURE(info, "globals were lost or not passed to the gui subsystem");

	// owned by the QML engine
	return new EngineLink{nullptr, info->engine};
}

QStringList EngineLink::get_global_binds() const {
	return this->global_binds;
}

void EngineLink::on_global_binds_changed(const std::vector<std::string>& global_binds) {
	QStringList new_global_binds;
	std::transform(std::begin(global_binds), std::end(global_binds), std::back_inserter(new_global_binds), [] (const std::string &s) {return QString::fromStdString(s);});

	if (this->global_binds != new_global_binds) {
		this->global_binds = new_global_binds;
		emit this->global_binds_changed();
	}
}

}} // namespace openage::gui
