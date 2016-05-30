// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QStringList>

#include "guisys/link/gui_singleton_item.h"

QT_FORWARD_DECLARE_CLASS(QQmlEngine)
QT_FORWARD_DECLARE_CLASS(QJSEngine)

namespace openage {
class Engine;
namespace gui {
class EngineLink;
}} // namespace openage::gui

namespace qtsdl {
template<>
struct Wrap<openage::Engine> {
	using Type = openage::gui::EngineLink;
};

template<>
struct Unwrap<openage::gui::EngineLink> {
	using Type = openage::Engine;
};

} // namespace qtsdl

namespace openage {
namespace gui {

class EngineLink : public qtsdl::GuiSingletonItem {
	Q_OBJECT

	Q_PROPERTY(QStringList globalBinds READ get_global_binds NOTIFY global_binds_changed)

public:
	explicit EngineLink(QObject *parent, Engine *engine);
	virtual ~EngineLink();

	static QObject* provider(QQmlEngine*, QJSEngine*);

	template<typename U>
	U* get() const {
		return core;
	}

	QStringList get_global_binds() const;

signals:
	void global_binds_changed();

private slots:
	void on_global_binds_changed(const std::vector<std::string>& global_binds);

private:
	Engine *core;

	QStringList global_binds;
};

}} // namespace openage::gui
