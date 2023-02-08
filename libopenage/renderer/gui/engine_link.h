// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <QStringList>

#include "gui/guisys/link/gui_singleton_item.h"
#include "util/path.h"

QT_FORWARD_DECLARE_CLASS(QQmlEngine)
QT_FORWARD_DECLARE_CLASS(QJSEngine)

namespace openage {

namespace engine {
class Engine;
}

namespace renderer::gui {
class EngineLink;
} // namespace renderer::gui
} // namespace openage

namespace qtsdl {
template <>
struct Wrap<openage::engine::Engine> {
	using Type = openage::renderer::gui::EngineLink;
};

template <>
struct Unwrap<openage::renderer::gui::EngineLink> {
	using Type = openage::engine::Engine;
};

} // namespace qtsdl

namespace openage {
namespace renderer {
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

public:
	explicit EngineLink(QObject *parent, engine::Engine *engine);
	virtual ~EngineLink();

	static QObject *provider(QQmlEngine *, QJSEngine *);

	template <typename U>
	U *get() const {
		return core;
	}

	QStringList get_global_binds() const;

	Q_INVOKABLE void stop();

signals:
	void global_binds_changed();

private slots:
	void on_global_binds_changed(const std::vector<std::string> &global_binds);

private:
	engine::Engine *core;

	QStringList global_binds;
};

} // namespace gui
} // namespace renderer
} // namespace openage
