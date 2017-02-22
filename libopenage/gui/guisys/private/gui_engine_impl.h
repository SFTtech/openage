// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "../link/qml_engine_with_singleton_items_info.h"
#include "livereload/recursive_directory_watcher.h"

QT_FORWARD_DECLARE_CLASS(QQuickWindow)

namespace qtsdl {

class GuiRenderer;
class GuiRendererImpl;
class GuiImageProvider;
class GuiEngine;
struct GuiSingletonItemsInfo;

class GuiEngineImpl : public QObject {
	Q_OBJECT

public:
	explicit GuiEngineImpl(GuiRenderer *renderer,
	                       const std::vector<GuiImageProvider*> &image_providers=std::vector<GuiImageProvider*>(),
	                       GuiSingletonItemsInfo *singleton_items_info=nullptr);
	virtual ~GuiEngineImpl();

	static GuiEngineImpl* impl(GuiEngine *engine);

	QQmlEngine* get_qml_engine();

	void add_root_dir_path(const QString &root_dir_path);
	void remove_root_dir_path(const QString &root_dir_path);

signals:
	void reload();
	void rootDirsPathsChanged(const QStringList&);

public slots:
	void attach_to(GuiRendererImpl *renderer);
	void onReload();

private:
	GuiRendererImpl *renderer;

	QmlEngineWithSingletonItemsInfo engine;
	RecursiveDirectoryWatcher watcher;

	QStringList root_dirs_paths;
};

} // namespace qtsdl
