// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_subtree_impl.h"

#include <cassert>

#include <QCoreApplication>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include "gui_renderer_impl.h"
#include "gui_engine_impl.h"
#include "../link/gui_item.h"
#include "../public/gui_subtree.h"
#include "gui_event_queue_impl.h"

namespace qtsdl {

namespace {
QString find_sourcefile(const QString &source, const std::vector<std::string> &search_paths) {
	if (!QDir::isAbsolutePath(source))
		for (auto& path : search_paths) {
			QDir dir{QString::fromStdString(path)};

			if (dir.exists(source))
				return dir.filePath(source);
		}

	return source;
}

}

GuiSubtreeImpl::GuiSubtreeImpl(GuiRenderer *renderer, GuiEventQueue *game_logic_updater, GuiEngine *engine, const QString &source, const std::vector<std::string> &search_paths)
	:
	QObject{},
	renderer{},
	engine{},
	root{} {

	auto resolved_source = find_sourcefile(source, search_paths);

	QObject::connect(&this->game_logic_callback, &GuiCallback::process_blocking, this, &GuiSubtreeImpl::on_process_game_logic_callback_blocking, Qt::DirectConnection);
	QObject::connect(this, &GuiSubtreeImpl::process_game_logic_callback_blocking, &this->game_logic_callback, &GuiCallback::process, QCoreApplication::instance()->thread() != QThread::currentThread() ? Qt::BlockingQueuedConnection : Qt::DirectConnection);

	this->moveToThread(QCoreApplication::instance()->thread());
	this->attach_to(GuiEventQueueImpl::impl(game_logic_updater));
	this->attach_to(GuiRendererImpl::impl(renderer));
	this->attach_to(GuiEngineImpl::impl(engine), resolved_source);

	assert(this->root_component);
	// Need to queue the loading because some underlying game logic elements require the loop to be running (maybe some things that are created after the gui).
	QMetaObject::invokeMethod(this->root_component.get(), "loadUrl", Qt::QueuedConnection, Q_ARG(QUrl, resolved_source));
}

GuiSubtreeImpl::~GuiSubtreeImpl() {
}

void GuiSubtreeImpl::onEngineReloaded() {
	const QUrl source = this->root_component->url();

	destroy_root();

	this->root_component = std::make_unique<QQmlComponent>(this->engine.get_qml_engine());
	QObject::connect(&*this->root_component, &QQmlComponent::statusChanged, this, &GuiSubtreeImpl::component_status_changed);
	this->root_component->loadUrl(source);
}

void GuiSubtreeImpl::attach_to(GuiEventQueueImpl *game_logic_updater) {
	this->game_logic_callback.moveToThread(game_logic_updater->get_thread());
}

void GuiSubtreeImpl::attach_to(GuiRendererImpl *renderer) {
	assert(renderer);

	if (this->renderer)
		QObject::disconnect(this->renderer, 0, this, 0);

	this->renderer = renderer;

	QObject::connect(this->renderer, &GuiRendererImpl::resized, this, &GuiSubtreeImpl::on_resized);
	this->reparent_root();
}

void GuiSubtreeImpl::attach_to(GuiEngineImpl *engine, const QString &source) {
	if (this->engine) {
		destroy_root();
		this->engine = GuiEngineImplConnection{};
	}

	this->root_component = std::make_unique<QQmlComponent>(engine->get_qml_engine());
	QObject::connect(&*this->root_component, &QQmlComponent::statusChanged, this, &GuiSubtreeImpl::component_status_changed);
	this->engine = GuiEngineImplConnection(this, engine, source);

	this->root_component->moveToThread(QCoreApplication::instance()->thread());
}

void GuiSubtreeImpl::component_status_changed(QQmlComponent::Status status) {
	if (QQmlComponent::Error == status) {
		qCritical("%s", qUtf8Printable(this->root_component->errorString()));
		return;
	}

	if (QQmlComponent::Ready == status) {
		assert(!this->root);

		this->root = qobject_cast<QQuickItem*>(this->root_component->beginCreate(this->engine.rootContext()));
		assert(this->root);

		this->init_persistent_items();

		this->root_component->completeCreate();

		this->reparent_root();
	}
}

void GuiSubtreeImpl::on_resized(const QSize &size) {
	if (this->root)
		this->root->setSize(size);
}

void GuiSubtreeImpl::on_process_game_logic_callback_blocking(const std::function<void()> &f) {
	TemporaryDisableGuiRendererSync {*this->renderer};
	emit this->process_game_logic_callback_blocking(f);
}

void GuiSubtreeImpl::init_persistent_items() {
	auto persistent = this->root->findChildren<GuiLiveReloaderAttachedProperty*>();

	for (auto ap : persistent)
		ap->get_attachee()->set_game_logic_callback(&this->game_logic_callback);

	this->reloader.init_persistent_items(persistent);
}

void GuiSubtreeImpl::reparent_root() {
	if (this->root) {
		QQuickWindow *window = this->renderer->get_window();

		this->root->setParentItem(window->contentItem());
		this->root->setSize(QSize{window->width(), window->height()});
	}
}

void GuiSubtreeImpl::destroy_root() {
	if (this->root) {
		this->root->setParent(nullptr);
		this->root->setParentItem(nullptr);
		this->root->deleteLater();
		this->root = nullptr;
	}
}

GuiEngineImplConnection::GuiEngineImplConnection()
	:
	subtree{},
	engine{} {
}

GuiEngineImplConnection::GuiEngineImplConnection(GuiSubtreeImpl *subtree, GuiEngineImpl *engine, const QString &source)
	:
	subtree(subtree),
	engine(engine) {

	assert(this->subtree);
	assert(this->engine);

	QObject::connect(this->engine, &GuiEngineImpl::reload, this->subtree, &GuiSubtreeImpl::onEngineReloaded);

	QString dir = QFileInfo(source).absolutePath();

	if (dir.isEmpty()) {
		qWarning() << "Can't determine the root dir of the QML file.";
	} else {
		this->engine->add_root_dir_path(this->root_dir = dir);
	}
}

GuiEngineImplConnection::~GuiEngineImplConnection() {
	this->disconnect();
}

void GuiEngineImplConnection::disconnect() {
	if (*this) {
		assert(this->engine);
		QObject::disconnect(this->engine, &GuiEngineImpl::reload, this->subtree, &GuiSubtreeImpl::onEngineReloaded);

		if (!this->root_dir.isEmpty())
			this->engine->remove_root_dir_path(this->root_dir);
	}
}

GuiEngineImplConnection::GuiEngineImplConnection(GuiEngineImplConnection &&cnx)
	:
	subtree{cnx.subtree},
	engine{cnx.engine} {

	cnx.subtree = nullptr;
	cnx.engine = nullptr;
}

GuiEngineImplConnection& GuiEngineImplConnection::operator=(GuiEngineImplConnection &&cnx) {
	this->disconnect();

	this->subtree = cnx.subtree;
	this->engine = cnx.engine;

	cnx.subtree = nullptr;
	cnx.engine = nullptr;

	return *this;
}

GuiEngineImplConnection::operator bool() const {
	return this->subtree;
}

QQmlContext* GuiEngineImplConnection::rootContext() const {
	assert(this->subtree);
	assert(this->engine);
	return this->engine->get_qml_engine()->rootContext();
}

QQmlEngine* GuiEngineImplConnection::get_qml_engine() const {
	assert(this->subtree);
	assert(this->engine);
	return this->engine->get_qml_engine();
}

} // namespace qtsdl
