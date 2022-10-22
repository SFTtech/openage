// Copyright 2018-2019 the openage authors. See copying.md for legal info.

#include "window.h"

#include "../../error/error.h"
#include "../../log/log.h"
#include "../sdl_global.h"
#include "../window_event_handler.h"

#include "gui/guisys/public/gui_application.h"
#include "renderer.h"
#include "context.h"
#include "context_qt.h"

#include <QGuiApplication>
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QString>


namespace openage::renderer::opengl {

GlWindow::GlWindow(const std::string &title, size_t width, size_t height) :
	Window{width, height} {

	// ASDF: Qt port
	if (QGuiApplication::instance() == nullptr) {
		// Qt windows need to attach to a QtGuiApplication
		throw Error{MSG(err) << "Failed to create Qt window: QGuiApplication has not been created yet."};
	}

	QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
	this->window = std::make_shared<QWindow>();

	this->window->setTitle(QString::fromStdString(title));
	this->window->resize(width, height);

	this->window->setSurfaceType(QSurface::OpenGLSurface);

	this->context = std::make_shared<QGlContext>(this->window);
	if (not this->context->get_raw_context()->isValid()) {
		throw Error{MSG(err) << "Failed to create Qt OpenGL context."};
	}

	this->add_resize_callback([](size_t w, size_t h) {
		glViewport(0, 0, w, h);
	});

	this->window->installEventFilter(this->event_handler.get());

	this->window->setVisible(true);
	log::log(MSG(info) << "Created Qt window with OpenGL context.");

	QGlContext::check_error();
}


void GlWindow::set_size(size_t width, size_t height) {
	if (this->size[0] != width || this->size[1] != height) {
		// ASDF: Qt port
		this->window->resize(width, height);
		this->size = {width, height};
	}

	for (auto &cb : this->on_resize) {
		cb(width, height);
	}
}


void GlWindow::update() {
	// ASDF: move this event handling to presenter

	auto events = this->event_handler->pop_events();

	for (auto &&event : events) {
		std::cout << "processing event type=" << event->type() << std::endl;

		switch (event->type()) {
		case QEvent::Resize: {
			// TODO: resize can be handled before the engine event processing
			auto ev = std::dynamic_pointer_cast<QResizeEvent>(event);
			size_t width = ev->size().width();
			size_t height = ev->size().height();
			log::log(MSG(dbg) << "Window resized to: " << width << "x" << height);

			this->size = {width, height};
			for (auto &cb : this->on_resize) {
				cb(width, height);
			}
		} break;
		case QEvent::Close: {
			this->should_be_closed = true;
		} break;

		case QEvent::KeyRelease: {
			auto const ev = std::dynamic_pointer_cast<QKeyEvent>(event);
			for (auto &cb : this->on_key) {
				cb(*ev);
			}
		} break;
		case QEvent::MouseButtonRelease: {
			auto const ev = std::dynamic_pointer_cast<QMouseEvent>(event);
			for (auto &cb : this->on_mouse_button) {
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
	return std::make_shared<GlRenderer>(this->get_context());
}


void GlWindow::make_context_current() {
	// SDL_GL_MakeCurrent(this->window.get(), this->context->get_raw_context());
	this->context->get_raw_context()->makeCurrent(this->window.get());
}


const std::shared_ptr<opengl::QGlContext> &GlWindow::get_context() const {
	return this->context;
}


} // namespace openage::renderer::opengl
