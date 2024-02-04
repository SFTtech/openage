// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include <QObject>
#include <QStringList>

#include "renderer/gui/guisys/private/livereload/recursive_directory_watcher.h"

QT_FORWARD_DECLARE_CLASS(QQmlEngine)

namespace qtgui {
class GuiQmlEngine;
class GuiRenderer;
class GuiRendererImpl;

/**
 * Qt QML engine wrapper.
 */
class GuiQmlEngineImpl : public QObject {
	Q_OBJECT

public:
	/**
	 * Create a QmlEngine and attach it to a GUI renderer.
	 *
	 * @param renderer Renderer for drawing the GUI. The renderer holds a
	 * 				   reference to an offscreen QQuickWindow that the QML
	 *				   components can be attached to.
	 */
	GuiQmlEngineImpl(std::shared_ptr<GuiRenderer> renderer);
	virtual ~GuiQmlEngineImpl();

	static GuiQmlEngineImpl *impl(GuiQmlEngine *engine);

	/**
	 * Get the underlying QQmlEngine object.
	 */
	std::shared_ptr<QQmlEngine> get_qml_engine();

	/**
	 * Add a path to the list of root dirs for GUI resources.
	 *
	 * @param root_dir_path Path to the resources.
	 */
	void add_root_dir_path(const QString &root_dir_path);

	/**
	 * Remove an existing path to the list of root dirs for GUI resources.
	 *
	 * @param root_dir_path Path to the resources.
	 */
	void remove_root_dir_path(const QString &root_dir_path);

signals:
	/**
	 * Qt signal emitted on reload.
	 */
	void reload();

	/**
	 * Qt signal emitted when the root dir paths are changed.
	 */
	void rootDirsPathsChanged(const QStringList &);

public slots:
	/**
	 * Qt slot for attaching a new GUI renderer.
	 *
	 * @param renderer Renderer for drawing the GUI.
	 */
	void attach_to(std::shared_ptr<GuiRenderer> renderer);

	/**
	 * Qt slot for reloading the GUI. Clears the component cache.
	 */
	void onReload();

private:
	/**
	 * GUI renderer that draws the QML.
	 */
	std::shared_ptr<GuiRenderer> renderer;

	/**
	 * Underlying Qt QML engine.
	 */
	std::shared_ptr<QQmlEngine> engine;

	/**
	 * Watcher for changes to the root directories. Used for
	 * live reloading the GUI if files are changed at runtime.
	 */
	RecursiveDirectoryWatcher watcher;

	/**
	 * Root directories for the QML assets.
	 */
	QStringList root_dirs_paths;
};

} // namespace qtgui
