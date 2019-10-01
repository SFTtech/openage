// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "engine_link.h"

#include <QtQml>

#include "../error/error.h"

#include "../engine.h"

#include "guisys/link/qml_engine_with_singleton_items_info.h"
#include "guisys/link/qtsdl_checked_static_cast.h"

namespace openage {
namespace gui {

namespace {
// this pushes the EngineLink in the QML engine.
// a qml engine calls the static provider() to obtain a handle.
const int registration = qmlRegisterSingletonType<EngineLink>("yay.sfttech.openage", 1, 0, "Engine", &EngineLink::provider);
}


EngineLink::EngineLink(QObject *parent, Engine *engine)
	:
	GuiSingletonItem{parent},
	core{engine} {
	Q_UNUSED(registration);

	ENSURE(!unwrap(this)->gui_link, "Sharing singletons between QML engines is not supported for now.");

	// when the engine announces that the global key bindings
	// changed, update the display.
	QObject::connect(
		&unwrap(this)->gui_signals,
		&EngineSignals::global_binds_changed,
		this,
		&EngineLink::on_global_binds_changed
	);

	// trigger the engine signal,
	// which then triggers this->on_global_binds_changed.
	unwrap(this)->announce_global_binds();
}

EngineLink::~EngineLink() {
	unwrap(this)->gui_link = nullptr;
}

// a qml engine requests a handle to the engine link with that static
// method we do this by extracting the per-qmlengine singleton from the
// engine (the qmlenginewithsingletoninfo), then just return the new link
// instance
QObject* EngineLink::provider(QQmlEngine *engine, QJSEngine*) {

	// cast the engine to our specialization
	qtsdl::QmlEngineWithSingletonItemsInfo *engine_with_singleton_items_info = qtsdl::checked_static_cast<qtsdl::QmlEngineWithSingletonItemsInfo*>(engine);

	// get the singleton container out of the custom qml engine
	auto info = static_cast<gui::EngineQMLInfo*>(
		engine_with_singleton_items_info->get_singleton_items_info()
	);
	ENSURE(info, "qml-globals were lost or not passed to the gui subsystem");

	// owned by the QML engine
	// this handle contains the pointer to the openage engine,
	// obtained through the qmlengine
	return new EngineLink{nullptr, info->engine};
}

QStringList EngineLink::get_global_binds() const {
	return this->global_binds;
}

void EngineLink::stop() {
	this->core->stop();
}

void EngineLink::on_global_binds_changed(const std::vector<std::string>& global_binds) {
	QStringList new_global_binds;

	// create the qstring list from the std string list
	// which is then displayed in the ui
	std::transform(
		std::begin(global_binds),
		std::end(global_binds),
		std::back_inserter(new_global_binds),
		[] (const std::string &s) {
			return QString::fromStdString(s);
		}
	);

	new_global_binds.sort();

	if (this->global_binds != new_global_binds) {
		this->global_binds = new_global_binds;
		emit this->global_binds_changed();
	}
}

}} // namespace openage::gui
