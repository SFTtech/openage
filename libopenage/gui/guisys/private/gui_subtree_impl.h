// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>
#include <string>

#include <QObject>
#include <QQmlComponent>

#include "gui_callback.h"
#include "livereload/gui_live_reloader.h"

QT_FORWARD_DECLARE_CLASS(QQuickItem)

namespace qtsdl {

class GuiRenderer;
class GuiRendererImpl;
class GuiEventQueue;
class GuiEventQueueImpl;
class GuiEngine;
class GuiEngineImpl;
class GuiSubtreeImpl;

class GuiEngineImplConnection {
public:
	GuiEngineImplConnection();
	explicit GuiEngineImplConnection(GuiSubtreeImpl *subtree,
	                                 GuiEngineImpl *engine,
	                                 QString root_dir);
	~GuiEngineImplConnection();

	GuiEngineImplConnection(GuiEngineImplConnection &&cnx) noexcept;
	GuiEngineImplConnection& operator=(GuiEngineImplConnection &&cnx) noexcept;

	bool has_subtree() const;

	QQmlContext* rootContext() const;
	QQmlEngine* get_qml_engine() const;

private:
	GuiEngineImplConnection(const GuiEngineImplConnection &cnx) = delete;
	GuiEngineImplConnection& operator=(const GuiEngineImplConnection &cnx) = delete;

	void disconnect();

	GuiSubtreeImpl *subtree;
	GuiEngineImpl *engine;
	QString root_dir;
};


class GuiSubtreeImpl : public QObject {
	Q_OBJECT

public:
	explicit GuiSubtreeImpl(GuiRenderer *renderer,
	                        GuiEventQueue *game_logic_updater,
	                        GuiEngine *engine,
	                        const QString &source,
	                        const QString &rootdir);
	virtual ~GuiSubtreeImpl();

public slots:
	void onEngineReloaded();

	void attach_to(GuiEventQueueImpl *game_logic_updater);
	void attach_to(GuiRendererImpl *renderer);
	void attach_to(GuiEngineImpl *engine, const QString &source);

private slots:
	void component_status_changed(QQmlComponent::Status status);
	void on_resized(const QSize &size);
	void on_process_game_logic_callback_blocking(const std::function<void()> &f);

signals:
	void process_game_logic_callback_blocking(const std::function<void()> &f);

private:
	void init_persistent_items();

	void reparent_root();
	void destroy_root();

	GuiRendererImpl *renderer;
	GuiEngineImplConnection engine;
	GuiLiveReloader reloader;

	GuiCallback game_logic_callback;

	std::unique_ptr<QQmlComponent> root_component;
	QQuickItem *root;
};

} // namespace qtsdl
