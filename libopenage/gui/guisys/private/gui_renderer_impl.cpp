// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_renderer_impl.h"

#include <cassert>

#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QQuickRenderControl>
#include <QQuickItem>
#include <QCoreApplication>
#include <QThread>

#include "../public/gui_renderer.h"
#include "platforms/context_extraction.h"


namespace qtsdl {

namespace {
const int registration = qRegisterMetaType<std::atomic<bool>*>("atomic_bool_ptr");
}

EventHandlingQuickWindow::EventHandlingQuickWindow(QQuickRenderControl *render_control)
	:
	QQuickWindow{render_control},
	focused_item{} {
	Q_UNUSED(registration);
}

EventHandlingQuickWindow::~EventHandlingQuickWindow() {
}

void EventHandlingQuickWindow::on_input_event(std::atomic<bool> *processed, QEvent *event, bool only_if_grabbed) {
	if (!only_if_grabbed || this->mouseGrabberItem()) {
		if (this->focused_item && (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)) {
			QCoreApplication::instance()->sendEvent(this->focused_item, event);
		} else {
			QCoreApplication::instance()->sendEvent(this, event);

			auto change_focus = [this] (QQuickItem *item) {
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

GuiRendererImpl::GuiRendererImpl(SDL_Window *window)
	:
	QObject{},
	need_fbo_resize{true},
	need_sync{},
	need_render{},
	gui_locked{},
	renderer_waiting_on_cond{} {

	this->moveToThread(QCoreApplication::instance()->thread());

	QVariant handle;
	WId id;

	std::tie(handle, id) = extract_native_context(window);

	// pass the SDL opengl context so qt can use it
	this->ctx = std::make_unique<QOpenGLContext>();
	this->ctx->setNativeHandle(handle);
	this->ctx->create();
	assert(this->ctx->isValid());

	// reuse the sdl window
	QWindow *w = QWindow::fromWinId(id);
	w->setSurfaceType(QSurface::OpenGLSurface);

	if (!this->ctx->makeCurrent(w)) {
		assert(false);
		return;
	}

	QObject::connect(&this->render_control, &QQuickRenderControl::renderRequested, [&] () {
		this->need_render = true;
	});

	QObject::connect(&this->render_control, &QQuickRenderControl::sceneChanged, this, &GuiRendererImpl::on_scene_changed);

	this->window = std::make_unique<EventHandlingQuickWindow>(&this->render_control);
	this->window->moveToThread(QCoreApplication::instance()->thread());
	QObject::connect(this, &GuiRendererImpl::resized, this->window.get(), &EventHandlingQuickWindow::on_resized);
	this->window->setClearBeforeRendering(true);
	this->window->setColor(QColor{0, 0, 0, 0});

	QObject::connect(&*this->window, &QQuickWindow::sceneGraphInitialized, this, [this] {
		std::tie(this->new_fbo_width, this->new_fbo_height) = std::make_tuple(this->window->width(), this->window->height());
		this->need_fbo_resize = true;
	});

	QObject::connect(&*this->window, &QQuickWindow::widthChanged, [this] { this->new_fbo_width = this->window->width(); this->need_fbo_resize = true; });
	QObject::connect(&*this->window, &QQuickWindow::heightChanged, [this] { this->new_fbo_height = this->window->height(); this->need_fbo_resize = true; });

	this->render_control.initialize(&*this->ctx);

	assert(this->ctx->isValid());
}

void GuiRendererImpl::on_scene_changed() {
	this->need_sync = true;
	this->need_render = true;
	this->render_control.polishItems();
}

void GuiRendererImpl::reinit_fbo_if_needed() {
	assert(QThread::currentThread() == this->ctx->thread());

	if (this->need_fbo_resize) {
		this->fbo = std::make_unique<QOpenGLFramebufferObject>(QSize(this->new_fbo_width, this->new_fbo_height), QOpenGLFramebufferObject::CombinedDepthStencil);
		this->window->setRenderTarget(&*this->fbo);
		this->need_fbo_resize = false;
	}

	assert(this->fbo);
}

GuiRendererImpl::~GuiRendererImpl() {
	// TODO: MAYBE:
	//       the this->ctx member frees the
	//       gl context even though that's SDL's job.
	//
	// the qt doc says that a native context isn't destroyed!
	// but somehow it is lost or destroyed!
	// https://doc.qt.io/qt-5/qopenglcontext.html#setNativeHandle
}

GuiRendererImpl* GuiRendererImpl::impl(GuiRenderer *renderer) {
	return renderer->impl.get();
}

GLuint GuiRendererImpl::render() {
	this->reinit_fbo_if_needed();

	// QQuickRenderControl::sync() must be called from the render thread while the gui thread is stopped.
	if (this->need_sync) {
		if (QCoreApplication::instance()->thread() != QThread::currentThread()) {
			std::unique_lock<std::mutex> lck{this->gui_guard};

			if (this->need_sync) {
				QCoreApplication::instance()->postEvent(this, new QEvent{QEvent::User}, INT_MAX);

				this->renderer_waiting_on_cond = true;
				this->gui_locked_cond.wait(lck, [this] {return this->gui_locked;});
				this->renderer_waiting_on_cond = false;

				this->render_control.sync();

				this->need_sync = false;
				this->gui_locked = false;

				lck.unlock();
				this->gui_locked_cond.notify_one();
			}
		} else {
			this->render_control.sync();
		}
	}

	this->render_control.render();

	this->window->resetOpenGLState();

	return this->fbo->texture();
}

void GuiRendererImpl::make_sure_render_thread_unlocked() {
	assert(QThread::currentThread() == QCoreApplication::instance()->thread());

	if (this->need_sync && QThread::currentThread() != this->render_control.thread()) {
		std::unique_lock<std::mutex> lck{this->gui_guard};

		if (this->renderer_waiting_on_cond) {
			this->process_freeze(std::move(lck));
			QCoreApplication::instance()->removePostedEvents(this, QEvent::User);
		}
	}
}

bool GuiRendererImpl::make_sure_render_thread_wont_sync() {
	assert(QThread::currentThread() == QCoreApplication::instance()->thread());

	if (this->need_sync && QThread::currentThread() != this->render_control.thread()) {
		std::unique_lock<std::mutex> lck{this->gui_guard};

		if (this->renderer_waiting_on_cond) {
			this->process_freeze(std::move(lck));
			QCoreApplication::instance()->removePostedEvents(this, QEvent::User);
			assert(!this->need_sync);
		} else {
			assert(this->need_sync);
			this->need_sync = false;
			return true;
		}
	}

	return false;
}

void GuiRendererImpl::demand_sync() {
	assert(QThread::currentThread() == QCoreApplication::instance()->thread());
	this->need_sync = true;
}

bool GuiRendererImpl::event(QEvent *e) {
	if (e->type() == QEvent::User) {
		std::unique_lock<std::mutex> lck{this->gui_guard};
		this->process_freeze(std::move(lck));
		return true;
	} else {
		return this->QObject::event(e);
	}
}

void GuiRendererImpl::process_freeze(std::unique_lock<std::mutex> lck) {
	this->gui_locked = true;

	lck.unlock();
	this->gui_locked_cond.notify_one();

	lck.lock();
	this->gui_locked_cond.wait(lck, [this] {return !this->gui_locked;});
}

EventHandlingQuickWindow* GuiRendererImpl::get_window() {
    return &*this->window;
}

void GuiRendererImpl::resize(const QSize &size) {
	emit this->resized(size);
}

TemporaryDisableGuiRendererSync::TemporaryDisableGuiRendererSync(GuiRendererImpl &renderer)
	:
	renderer{renderer},
	need_sync{renderer.make_sure_render_thread_wont_sync()} {
}

TemporaryDisableGuiRendererSync::~TemporaryDisableGuiRendererSync() {
	if (this->need_sync)
		renderer.demand_sync();
}

} // namespace qtsdl
