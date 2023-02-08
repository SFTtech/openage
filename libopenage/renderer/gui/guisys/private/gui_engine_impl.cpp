// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#include "gui_engine_impl.h"

#include <cassert>

#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>
#include <QQuickWindow>

#include "renderer/gui/guisys/private/gui_renderer_impl.h"
#include "renderer/gui/guisys/public/gui_engine.h"

namespace qtgui {

GuiQmlEngineImpl::GuiQmlEngineImpl(std::shared_ptr<GuiRenderer> renderer) :
	QObject{},
	renderer{},
	engine{std::make_shared<QQmlEngine>()} {
	// QThread *gui_thread = QCoreApplication::instance()->thread();
	// this->moveToThread(gui_thread);
	// this->engine.moveToThread(gui_thread);
	// this->watcher.moveToThread(gui_thread);

	assert(!this->engine->incubationController());
	this->attach_to(renderer);

	QObject::connect(this,
	                 &GuiQmlEngineImpl::rootDirsPathsChanged,
	                 &this->watcher,
	                 &RecursiveDirectoryWatcher::rootDirsPathsChanged);
	QObject::connect(&this->watcher,
	                 &RecursiveDirectoryWatcher::changeDetected,
	                 this,
	                 &GuiQmlEngineImpl::onReload);
}

GuiQmlEngineImpl::~GuiQmlEngineImpl() = default;

GuiQmlEngineImpl *GuiQmlEngineImpl::impl(GuiQmlEngine *engine) {
	return engine->impl.get();
}

void GuiQmlEngineImpl::attach_to(std::shared_ptr<GuiRenderer> renderer) {
	this->renderer = renderer;
	auto renderer_impl = GuiRendererImpl::impl(renderer.get());
	this->engine->setIncubationController(renderer_impl->get_window()->incubationController());
}

std::shared_ptr<QQmlEngine> GuiQmlEngineImpl::get_qml_engine() {
	return this->engine;
}

void GuiQmlEngineImpl::onReload() {
	qDebug("reloading GUI");
	this->engine->clearComponentCache();
	emit this->reload();
}

void GuiQmlEngineImpl::add_root_dir_path(const QString &root_dir_path) {
	this->root_dirs_paths.push_back(root_dir_path);
	emit this->rootDirsPathsChanged(this->root_dirs_paths);
}

void GuiQmlEngineImpl::remove_root_dir_path(const QString &root_dir_path) {
	if (this->root_dirs_paths.removeOne(root_dir_path))
		emit this->rootDirsPathsChanged(this->root_dirs_paths);
	else
		qWarning() << "Failed to remove path watched by ReloadableQmlEngine.";
}

} // namespace qtgui
