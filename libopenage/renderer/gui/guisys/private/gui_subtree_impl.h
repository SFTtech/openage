// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <QObject>
#include <QQmlComponent>

QT_FORWARD_DECLARE_CLASS(QQuickItem)

namespace qtgui {

class GuiRenderer;
class GuiQmlEngine;

class GuiSubtreeImpl : public QObject {
	Q_OBJECT

public:
	explicit GuiSubtreeImpl(std::shared_ptr<GuiRenderer> renderer,
	                        std::shared_ptr<GuiQmlEngine> engine,
	                        const QString &source,
	                        const QString &rootdir);
	virtual ~GuiSubtreeImpl();

public slots:
	void onEngineReloaded();

	// void attach_to(GuiEventQueueImpl *game_logic_updater);
	void attach_to(std::shared_ptr<GuiRenderer> renderer);
	void attach_to(std::shared_ptr<GuiQmlEngine> engine, const QString &rootdir);

private slots:
	void component_status_changed(QQmlComponent::Status status);
	void on_resized(const QSize &size);

signals:
	void process_game_logic_callback_blocking(const std::function<void()> &f);

private:
	void reparent_root();
	void destroy_root();

	std::shared_ptr<GuiRenderer> renderer;
	std::shared_ptr<GuiQmlEngine> engine;
	// GuiLiveReloader reloader;

	std::unique_ptr<QQmlComponent> root_component;
	std::shared_ptr<QQuickItem> root;
	QString root_dir;
};

// class GuiEngineImplConnection {
// public:
// 	GuiEngineImplConnection();
// 	explicit GuiEngineImplConnection(GuiSubtreeImpl *subtree,
// 	                                 GuiEngineImpl *engine,
// 	                                 QString root_dir);
// 	~GuiEngineImplConnection();

// 	GuiEngineImplConnection(GuiEngineImplConnection &&cnx) noexcept;
// 	GuiEngineImplConnection &operator=(GuiEngineImplConnection &&cnx) noexcept;

// 	bool has_subtree() const;

// 	QQmlContext *rootContext() const;
// 	QQmlEngine *get_qml_engine() const;

// private:
// 	GuiEngineImplConnection(const GuiEngineImplConnection &cnx) = delete;
// 	GuiEngineImplConnection &operator=(const GuiEngineImplConnection &cnx) = delete;

// 	void disconnect();

// 	GuiSubtreeImpl *subtree;
// 	GuiEngineImpl *engine;
// 	QString root_dir;
// };


} // namespace qtgui
