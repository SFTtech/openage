// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <QStringList>

#include "../util/path.h"
#include "guisys/link/gui_singleton_item.h"

#include "cvar_manager_link.h"
#include "gui_global_font.h"

QT_FORWARD_DECLARE_CLASS(QQmlEngine)
QT_FORWARD_DECLARE_CLASS(QJSEngine)

namespace openage {
class Engine;

namespace gui {
class EngineLink;
} // gui
} // openage

namespace qtsdl {
class QmlEngineWithSingletonItemsInfo;

template<>
struct Wrap<openage::Engine> {
	using Type = openage::gui::EngineLink;
};

template<>
struct Unwrap<openage::gui::EngineLink> {
	using Type = openage::Engine;
};

} // qtsdl

namespace openage {
namespace gui {

class EngineLink : public qtsdl::GuiSingletonItem {
	Q_OBJECT

	/**
	 * The text list of global key bindings.
	 * displayed so one can see what keys are active.
	 */
	Q_PROPERTY(QStringList globalBinds
	           READ get_global_binds
	           NOTIFY global_binds_changed)

	Q_PROPERTY(openage::gui::CVarManagerLink* cvarManager READ get_cvar_manager CONSTANT)

public:
	explicit EngineLink(QObject *parent, Engine *engine, qtsdl::QmlEngineWithSingletonItemsInfo *engine_with_singleton_items_info);
	virtual ~EngineLink();

	static QObject* provider(QQmlEngine*, QJSEngine*);

	template<typename U>
	U* get() const {
		return core;
	}

	QStringList get_global_binds() const;

	CVarManagerLink* get_cvar_manager();

signals:
	void global_binds_changed();

private slots:
	void on_global_binds_changed(const std::vector<std::string>& global_binds);

private:
	Engine *core;

	QStringList global_binds;
	GuiGlobalFont global_font;
	CVarManagerLink cvar_manager;
};

}} // openage::gui
