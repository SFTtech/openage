// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "gui_renderer_impl.h"

#include <cassert>

#include <QCoreApplication>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QQuickGraphicsDevice>
#include <QQuickItem>
#include <QQuickRenderControl>
#include <QQuickRenderTarget>
#include <QThread>

#include "log/log.h"

#include "renderer/gui/guisys/public/gui_renderer.h"
#include "renderer/window.h"


namespace qtgui {

namespace {
const int registration = qRegisterMetaType<std::atomic<bool> *>("atomic_bool_ptr");
}

EventHandlingQuickWindow::EventHandlingQuickWindow(QQuickRenderControl *render_control) :
	QQuickWindow{render_control},
	focused_item{} {
	Q_UNUSED(registration);
}

EventHandlingQuickWindow::~EventHandlingQuickWindow() = default;

void EventHandlingQuickWindow::on_input_event(std::atomic<bool> *processed, QEvent *event, bool only_if_grabbed) {
	if (!only_if_grabbed || this->mouseGrabberItem()) {
		if (this->focused_item && (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)) {
			QCoreApplication::instance()->sendEvent(this->focused_item, event);
		}
		else {
			QCoreApplication::instance()->sendEvent(this, event);

			auto change_focus = [this](QQuickItem *item) {
				if (this->focused_item != item) {
					if (this->focused_item) {
						QFocusEvent focus_out{QEvent::FocusOut, Qt::ActiveWindowFocusReason};
						QCoreApplication::instance()->sendEvent(this->focused_item, &focus_out);
					}

					if (item) {
						QFocusEvent focus_in{QEvent::FocusIn, Qt::ActiveWindowFocusReason};
						QCoreApplication::instance()->sendEvent(item, &focus_in);
					}
				}

				this->focused_item = item;
			};

			// Loose keyboard focus when clicked outside of gui.
			if (event->type() == QEvent::MouseButtonPress && !event->isAccepted())
				change_focus(nullptr);

			// Normally, the QQuickWindow would handle keyboard focus automatically, but it can't because neither QQuickWindow nor
			// its target QWindow respond to requestActivate(). Which means no focus event propagation when injecting mouse clicks.
			// So, the workaround is to look specifically for TextFields and give them focus directly.
			// TODO: to remove when the proper focus for the foreign (that obtained from QWindow::fromWinId()) windows is implemented (Qt 5.6).
			if (this->mouseGrabberItem() && this->mouseGrabberItem()->metaObject()->superClass() && this->mouseGrabberItem()->metaObject()->superClass()->className() == QString("QQuickTextInput") && (event->type() == QEvent::MouseButtonPress))
				change_focus(this->mouseGrabberItem());
		}
	}

	*processed = true;
}

void EventHandlingQuickWindow::on_resized(const QSize &size) {
	this->resize(size);
}

GuiRendererImpl::GuiRendererImpl(std::shared_ptr<openage::renderer::Window> window) :
	QObject{},
	render_control{},
	gui_context{window} {

	this->target_window = std::make_unique<QQuickWindow>();
	this->resize(window->get_qt_window()->size());

	QObject::connect(&*this->target_window, &QQuickWindow::sceneGraphInitialized, this, &GuiRendererImpl::create_texture);
	QObject::connect(&*this->target_window, &QQuickWindow::sceneGraphInvalidated, this, &GuiRendererImpl::destroy_texture);

	this->gui_context.pre_render();
	this->target_window->setGraphicsDevice(QQuickGraphicsDevice::fromOpenGLContext(this->gui_context.get_ctx().get()));
	this->render_control.initialize();
}

GuiRendererImpl::~GuiRendererImpl() {
	// Make sure the context is current while doing cleanup.
	this->gui_context.pre_render();

	if (this->target_texture_id) {
		this->gui_context.get_ctx()->functions()->glDeleteTextures(1, &this->target_texture_id);
	}
	this->gui_context.get_ctx()->doneCurrent();
}

void GuiRendererImpl::create_texture() {
	auto m_dpr = this->target_window->devicePixelRatio();
	auto m_textureSize = this->target_window->size() * m_dpr;
	QOpenGLFunctions *f = this->gui_context.get_ctx()->functions();
	f->glGenTextures(1, &this->target_texture_id);
	f->glBindTexture(GL_TEXTURE_2D, this->target_texture_id);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_textureSize.width(), m_textureSize.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	this->target_window->setRenderTarget(QQuickRenderTarget::fromOpenGLTexture(this->target_texture_id, m_textureSize));
}

void GuiRendererImpl::destroy_texture() {
	this->gui_context.get_ctx()->functions()->glDeleteTextures(1, &this->target_texture_id);
	this->target_texture_id = 0;
}

GLuint GuiRendererImpl::render() {
	this->gui_context.pre_render();

	this->render_control.beginFrame();
	this->render_control.polishItems();
	this->render_control.sync();
	this->render_control.render();
	this->render_control.endFrame();

	this->gui_context.post_render();

	return this->target_texture_id;
}

GuiRendererImpl *GuiRendererImpl::impl(GuiRenderer *renderer) {
	return renderer->impl.get();
}

QQuickWindow *GuiRendererImpl::get_window() {
	return &*this->target_window;
}

void GuiRendererImpl::resize(const size_t width, const size_t height) {
	this->target_window->setGeometry(0, 0, width, height);
	emit this->resized(QSize(width, height));
}

void GuiRendererImpl::resize(const QSize &size) {
	this->target_window->setGeometry(0, 0, size.width(), size.height());
	emit this->resized(size);
}

} // namespace qtgui
