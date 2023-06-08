// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "window.h"

#include "error/error.h"
#include "gui/guisys/public/gui_application.h"
#include "log/log.h"
#include "renderer/opengl/context.h"
#include "renderer/opengl/renderer.h"
#include "renderer/window_event_handler.h"

#include <QGuiApplication>
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QString>


namespace openage::renderer::opengl {

GlWindow::GlWindow(const std::string &title, size_t width, size_t height) :
	Window{width, height} {
	if (QGuiApplication::instance() == nullptr) {
		// Qt windows need to attach to a QtGuiApplication
		throw Error{MSG(err) << "Failed to create Qt window: QGuiApplication has not been created yet."};
	}

	QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
	this->window = std::make_shared<QWindow>();

	this->window->setTitle(QString::fromStdString(title));
	this->window->resize(width, height);

	this->window->setSurfaceType(QSurface::OpenGLSurface);

	this->context = std::make_shared<GlContext>(this->window);
	if (not this->context->get_raw_context()->isValid()) {
		throw Error{MSG(err) << "Failed to create Qt OpenGL context."};
	}

	this->window->installEventFilter(this->event_handler.get());

	this->window->setVisible(true);
	log::log(MSG(info) << "Created Qt window with OpenGL context.");

	// Scaling factor if highDPI
	this->scale_dpr = this->window->devicePixelRatio();

	GlContext::check_error();
}

GlWindow::~GlWindow() {
	this->make_context_current();
}


void GlWindow::set_size(size_t width, size_t height) {
	if (this->size[0] != width || this->size[1] != height) {
		this->window->resize(width, height);
		this->size = {width, height};
	}

	for (auto &cb : this->on_resize) {
		cb(width, height, this->scale_dpr);
	}
}


void GlWindow::update() {
	// TODO: move this event handling to presenter

	auto events = this->event_handler->pop_events();

	for (auto &&event : events) {
		switch (event->type()) {
		case QEvent::Resize: {
			// TODO: resize can be handled before the engine event processing
			auto ev = std::dynamic_pointer_cast<QResizeEvent>(event);
			size_t width = ev->size().width();
			size_t height = ev->size().height();
			log::log(MSG(dbg) << "Window resized to: " << width << "x" << height);

			this->size = {width, height};
			for (auto &cb : this->on_resize) {
				cb(width, height, this->scale_dpr);
			}
		} break;
		case QEvent::Close: {
			this->should_be_closed = true;
		} break;

		case QEvent::KeyPress:
		case QEvent::KeyRelease: {
			auto const ev = std::dynamic_pointer_cast<QKeyEvent>(event);
			for (auto &cb : this->on_key) {
				cb(*ev);
			}
		} break;
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
		case QEvent::MouseButtonDblClick: {
			auto const ev = std::dynamic_pointer_cast<QMouseEvent>(event);
			for (auto &cb : this->on_mouse_button) {
				cb(*ev);
			}
		} break;
		case QEvent::Wheel: {
			auto const ev = std::dynamic_pointer_cast<QWheelEvent>(event);
			for (auto &cb : this->on_mouse_wheel) {
				cb(*ev);
			}
		} break;
		default:
			break; // unhandled event
		}
	}

	this->context->get_raw_context()->swapBuffers(this->window.get());
}


std::shared_ptr<Renderer> GlWindow::make_renderer() {
	auto renderer = std::make_shared<GlRenderer>(this->get_context(),
	                                             this->size * this->scale_dpr);

	this->add_resize_callback([this, renderer](size_t w, size_t h, double scale) {
		// this up-scales all the default framebuffer to the "bigger" highdpi window.
		renderer->resize_display_target(w * scale, h * scale);
	});

	return renderer;
}


void GlWindow::make_context_current() {
	this->context->get_raw_context()->makeCurrent(this->window.get());
}


void GlWindow::done_context_current() {
	this->context->get_raw_context()->doneCurrent();
}


const std::shared_ptr<opengl::GlContext> &GlWindow::get_context() const {
	return this->context;
}


} // namespace openage::renderer::opengl
