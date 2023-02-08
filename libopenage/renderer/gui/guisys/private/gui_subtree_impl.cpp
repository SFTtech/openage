// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "gui_subtree_impl.h"
#include "gui_renderer_impl.h"

#include <cassert>
#include <ciso646>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <utility>

#include "renderer/gui/guisys/private/gui_engine_impl.h"
#include "renderer/gui/guisys/private/gui_renderer_impl.h"
#include "renderer/gui/guisys/public/gui_engine.h"
#include "renderer/gui/guisys/public/gui_subtree.h"

namespace qtgui {


GuiSubtreeImpl::GuiSubtreeImpl(std::shared_ptr<GuiRenderer> renderer,
                               std::shared_ptr<GuiQmlEngine> engine,
                               const QString &source,
                               const QString &rootdir) :
	QObject{},
	renderer{},
	engine{},
	root{} {
	// this->moveToThread(QCoreApplication::instance()->thread());

	// this->attach_to(GuiEventQueueImpl::impl(game_logic_updater));
	this->attach_to(renderer);
	this->attach_to(engine, rootdir);

	// Should now be initialized by the engine-attaching
	assert(this->root_component);
	this->root_component->loadUrl(QUrl::fromLocalFile(source));

	// Need to queue the loading because some underlying game logic elements
	// require the loop to be running (maybe some things that are created after
	// the gui).
	// QMetaObject::invokeMethod(this->root_component.get(),
	//                           "loadUrl",
	//                           Qt::QueuedConnection,
	//                           Q_ARG(QUrl, QUrl::fromLocalFile(source)));
}

GuiSubtreeImpl::~GuiSubtreeImpl() = default;

void GuiSubtreeImpl::onEngineReloaded() {
	const QUrl source = this->root_component->url();

	this->destroy_root();

	auto engine_impl = GuiQmlEngineImpl::impl(this->engine.get());
	this->root_component = std::make_unique<QQmlComponent>(engine_impl->get_qml_engine().get());

	QObject::connect(
		this->root_component.get(),
		&QQmlComponent::statusChanged,
		this,
		&GuiSubtreeImpl::component_status_changed);

	this->root_component->loadUrl(source);
}

// void GuiSubtreeImpl::attach_to(GuiEventQueueImpl *game_logic_updater) {
// 	this->game_logic_callback.moveToThread(game_logic_updater->get_thread());
// }

void GuiSubtreeImpl::attach_to(std::shared_ptr<GuiRenderer> renderer) {
	assert(renderer);

	auto renderer_impl = GuiRendererImpl::impl(renderer.get());
	if (this->renderer)
		QObject::disconnect(renderer_impl, nullptr, this, nullptr);

	this->renderer = renderer;

	QObject::connect(
		renderer_impl,
		&GuiRendererImpl::resized,
		this,
		&GuiSubtreeImpl::on_resized);
	this->reparent_root();
}

void GuiSubtreeImpl::attach_to(std::shared_ptr<GuiQmlEngine> engine, const QString &rootdir) {
	this->destroy_root();
	if (this->engine) {
		// disconnect the current engine
		auto engine_impl = GuiQmlEngineImpl::impl(this->engine.get());
		QObject::disconnect(
			engine_impl,
			&GuiQmlEngineImpl::reload,
			this,
			&GuiSubtreeImpl::onEngineReloaded);
		if (not this->root_dir.isEmpty()) {
			engine_impl->remove_root_dir_path(this->root_dir);
		}
	}

	this->engine = engine;

	auto engine_impl = GuiQmlEngineImpl::impl(engine.get());
	this->root_component = std::make_unique<QQmlComponent>(engine_impl->get_qml_engine().get());

	QObject::connect(
		this->root_component.get(),
		&QQmlComponent::statusChanged,
		this,
		&GuiSubtreeImpl::component_status_changed);

	QObject::connect(
		engine_impl,
		&GuiQmlEngineImpl::reload,
		this,
		&GuiSubtreeImpl::onEngineReloaded);

	this->root_dir = rootdir;
	engine_impl->add_root_dir_path(this->root_dir);

	// this->root_component->moveToThread(QCoreApplication::instance()->thread());
}

void GuiSubtreeImpl::component_status_changed(QQmlComponent::Status status) {
	if (QQmlComponent::Error == status) {
		qCritical("%s", qUtf8Printable(this->root_component->errorString()));
		return;
	}

	if (QQmlComponent::Ready == status) {
		assert(!this->root);

		auto engine_impl = GuiQmlEngineImpl::impl(this->engine.get());
		auto root_context = engine_impl->get_qml_engine()->rootContext();
		this->root = std::shared_ptr<QQuickItem>{
			qobject_cast<QQuickItem *>(this->root_component->beginCreate(root_context))};
		assert(this->root);

		// this->init_persistent_items();

		this->root_component->completeCreate();

		this->reparent_root();
	}
}

void GuiSubtreeImpl::on_resized(const QSize &size) {
	if (this->root)
		this->root->setSize(size);
}

void GuiSubtreeImpl::reparent_root() {
	if (this->root) {
		auto renderer_impl = GuiRendererImpl::impl(this->renderer.get());
		auto window = renderer_impl->get_window();

		this->root->setParentItem(window->contentItem());
		this->root->setSize(QSize{window->width(), window->height()});
		// delegate key input events to root object
		this->root->forceActiveFocus();
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

// GuiEngineImplConnection::GuiEngineImplConnection() :
// 	subtree{},
// 	engine{} {
// }

// GuiEngineImplConnection::GuiEngineImplConnection(GuiSubtreeImpl *subtree,
//                                                  GuiEngineImpl *engine,
//                                                  QString root_dir) :
// 	subtree{subtree},
// 	engine{engine},
// 	root_dir{std::move(root_dir)} {
// 	assert(this->subtree);
// 	assert(this->engine);

// 	QObject::connect(
// 		this->engine,
// 		&GuiEngineImpl::reload,
// 		this->subtree,
// 		&GuiSubtreeImpl::onEngineReloaded);

// 	// add the directory so it can be watched for changes.
// 	this->engine->add_root_dir_path(this->root_dir);
// }

// GuiEngineImplConnection::~GuiEngineImplConnection() {
// 	this->disconnect();
// }

// void GuiEngineImplConnection::disconnect() {
// 	if (this->has_subtree()) {
// 		assert(this->engine);

// 		QObject::disconnect(
// 			this->engine,
// 			&GuiEngineImpl::reload,
// 			this->subtree,
// 			&GuiSubtreeImpl::onEngineReloaded);

// 		if (not this->root_dir.isEmpty()) {
// 			this->engine->remove_root_dir_path(this->root_dir);
// 		}
// 	}
// }

// GuiEngineImplConnection::GuiEngineImplConnection(GuiEngineImplConnection &&cnx) noexcept
// 	:
// 	subtree{cnx.subtree},
// 	engine{cnx.engine} {
// 	cnx.subtree = nullptr;
// 	cnx.engine = nullptr;
// }

// GuiEngineImplConnection &GuiEngineImplConnection::operator=(GuiEngineImplConnection &&cnx) noexcept {
// 	this->disconnect();

// 	this->subtree = cnx.subtree;
// 	this->engine = cnx.engine;

// 	cnx.subtree = nullptr;
// 	cnx.engine = nullptr;

// 	return *this;
// }

// bool GuiEngineImplConnection::has_subtree() const {
// 	return this->subtree != nullptr;
// }

// QQmlContext *GuiEngineImplConnection::rootContext() const {
// 	assert(this->subtree);
// 	assert(this->engine);
// 	return this->engine->get_qml_engine()->rootContext();
// }

// QQmlEngine *GuiEngineImplConnection::get_qml_engine() const {
// 	assert(this->subtree);
// 	assert(this->engine);
// 	return this->engine->get_qml_engine();
// }

} // namespace qtgui
