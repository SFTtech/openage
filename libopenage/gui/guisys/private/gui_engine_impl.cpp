// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_engine_impl.h"

#include <cassert>

#include <QCoreApplication>
#include <QQuickWindow>
#include <QDebug>

#include "../public/gui_engine.h"
#include "gui_image_provider_impl.h"
#include "gui_renderer_impl.h"

namespace qtsdl {

GuiEngineImpl::GuiEngineImpl(GuiRenderer *renderer, const std::vector<GuiImageProvider*> &image_providers, GuiSingletonItemsInfo *singleton_items_info)
	:
	QObject{},
	renderer{},
	engine{GuiImageProviderImpl::take_ownership(image_providers), singleton_items_info} {

	QThread *gui_thread = QCoreApplication::instance()->thread();
	this->moveToThread(gui_thread);
	this->engine.moveToThread(gui_thread);
	this->watcher.moveToThread(gui_thread);

	assert(!this->engine.incubationController());
	this->attach_to(GuiRendererImpl::impl(renderer));

	QObject::connect(this, &GuiEngineImpl::rootDirsPathsChanged, &this->watcher, &RecursiveDirectoryWatcher::rootDirsPathsChanged);
	QObject::connect(&this->watcher, &RecursiveDirectoryWatcher::changeDetected, this, &GuiEngineImpl::onReload);	
}

GuiEngineImpl::~GuiEngineImpl() {
}

GuiEngineImpl* GuiEngineImpl::impl(GuiEngine *engine) {
	return engine->impl.get();
}

void GuiEngineImpl::attach_to(GuiRendererImpl *renderer) {
	this->renderer = renderer;
	this->engine.setIncubationController(this->renderer->get_window()->incubationController());
}

QQmlEngine* GuiEngineImpl::get_qml_engine() {
	return &this->engine;
}

void GuiEngineImpl::onReload() {
	qDebug("reloading GUI");
	this->engine.clearComponentCache();
	emit this->reload();
}

void GuiEngineImpl::add_root_dir_path(const QString &root_dir_path) {
	this->root_dirs_paths.push_back(root_dir_path);
	emit this->rootDirsPathsChanged(this->root_dirs_paths);
}

void GuiEngineImpl::remove_root_dir_path(const QString &root_dir_path) {
	if (this->root_dirs_paths.removeOne(root_dir_path))
		emit this->rootDirsPathsChanged(this->root_dirs_paths);
	else
		qWarning() << "Failed to remove path watched by ReloadableQmlEngine.";
}

} // namespace qtsdl
