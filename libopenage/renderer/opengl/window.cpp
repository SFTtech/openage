// Copyright 2018-2019 the openage authors. See copying.md for legal info.

#include "window.h"

#include "../../error/error.h"
#include "../../log/log.h"
#include "../sdl_global.h"

#include "gui/guisys/public/gui_application.h"
#include "renderer.h"
#include "renderer/event_handling_window.h"
#include "renderer/opengl/context.h"
#include "renderer/opengl/context_qt.h"

#include <QGuiApplication>
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QString>


namespace openage {
namespace renderer {
namespace opengl {

GlWindow::GlWindow(const std::string &title, size_t width, size_t height) :
	Window{width, height} {
	make_sdl_available();

	// We need HIGHDPI for eventual support of GUI scaling.
	// TODO HIGHDPI fails (requires newer SDL2?)
	int32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
	this->window = std::shared_ptr<SDL_Window>(
		SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			this->size[0],
			this->size[1],
			window_flags),
		[](SDL_Window *window) {
			log::log(MSG(info) << "Destroying SDL window...");
			SDL_DestroyWindow(window);
		});

	if (this->window == nullptr) {
		throw Error{MSG(err) << "Failed to create SDL window: " << SDL_GetError()};
	}
	this->context = std::make_shared<opengl::GlContext>(this->window);
	this->add_resize_callback([](size_t w, size_t h) { glViewport(0, 0, w, h); });

	log::log(MSG(info) << "Created SDL window with OpenGL context.");

	// ASDF: Qt port
	if (QGuiApplication::instance() == nullptr) {
		// Qt windows need to attach to a QtGuiApplication
		throw Error{MSG(err) << "Failed to create Qt window: QGuiApplication has not been created yet."};
	}

	QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
	this->qwindow = std::make_shared<renderer::EventHandlingQuickWindow>();
	this->qwindow->setTitle(QString::fromStdString(title));
	this->qwindow->resize(width, height);

	this->qwindow->setSurfaceType(QSurface::OpenGLSurface);

	// QSurfaceFormat format;
	// format.setDepthBufferSize(16);
	// format.setStencilBufferSize(8);
	// this->qwindow->setFormat(format);

	this->qcontext = std::make_shared<QGlContext>(this->qwindow);
	if (!this->qcontext->get_raw_context()->isValid()) {
		throw Error{MSG(err) << "Failed to create Qt OpenGL context."};
	}
	this->add_resize_callback([](size_t w, size_t h) { glViewport(0, 0, w, h); });

	this->qwindow->setVisible(true);
	log::log(MSG(info) << "Created Qt window with OpenGL context.");
}

std::shared_ptr<SDL_Window> GlWindow::get_sdl_window() {
	return this->window;
}

std::shared_ptr<QWindow> GlWindow::get_qt_window() {
	return this->qwindow;
}

void GlWindow::set_size(size_t width, size_t height) {
	if (this->size[0] != width || this->size[1] != height) {
		SDL_SetWindowSize(this->window.get(), width, height);
		this->size = {width, height};

		// ASDF: Qt port
		this->qwindow->resize(width, height);
		this->size = {width, height};
	}

	for (auto &cb : this->on_resize) {
		cb(width, height);
	}
}

void GlWindow::update() {
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				size_t width = event.window.data1;
				size_t height = event.window.data2;
				log::log(MSG(dbg) << "Window resized to: " << width << "x" << height);

				this->size = {width, height};
				for (auto &cb : this->on_resize) {
					cb(width, height);
				}
			}
		}
		else if (event.type == SDL_QUIT) {
			this->should_be_closed = true;
			// TODO call on_destroy
		}
		else if (event.type == SDL_KEYUP) {
			auto const ev = *reinterpret_cast<SDL_KeyboardEvent const *>(&event);
			for (auto &cb : this->on_key) {
				cb(ev);
			}
			// TODO handle keydown
		}
		else if (event.type == SDL_MOUSEBUTTONUP) {
			auto const ev = *reinterpret_cast<SDL_MouseButtonEvent const *>(&event);
			for (auto &cb : this->on_mouse_button) {
				cb(ev);
			}
			// TODO handle mousedown
		}
	}

	SDL_GL_SwapWindow(this->window.get());
}

void GlWindow::qupdate() {
	// ASDF: Qt port
	auto events = this->qwindow->poll_events();

	for (auto event : events) {
		if (event->type() == QEvent::Resize) {
			auto ev = reinterpret_cast<QResizeEvent const *>(event);
			size_t width = ev->size().width();
			size_t height = ev->size().height();
			log::log(MSG(dbg) << "Window resized to: " << width << "x" << height);

			this->size = {width, height};
			for (auto &cb : this->on_resize) {
				cb(width, height);
			}
		}
		else if (event->type() == QEvent::Quit) {
			this->should_be_closed = true;
			// TODO call on_destroy
		}
		else if (event->type() == QEvent::KeyRelease) {
			auto const ev = reinterpret_cast<QKeyEvent const *>(&event);
			for (auto &cb : this->on_key) {
				// cb(ev);
			}
			// TODO handle keydown
		}
		else if (event->type() == QEvent::MouseButtonRelease) {
			auto const ev = reinterpret_cast<QMouseEvent const *>(&event);
			for (auto &cb : this->on_mouse_button) {
				// cb(ev);
			}
			// TODO handle mousedown
		}
	}

	this->qcontext->get_raw_context()->swapBuffers(this->qwindow.get());
}


std::shared_ptr<Renderer> GlWindow::make_renderer() {
	return std::make_shared<GlRenderer>(this->get_context());
}

void GlWindow::make_context_current() {
	// SDL_GL_MakeCurrent(this->window.get(), this->context->get_raw_context());
	this->qcontext->get_raw_context()->makeCurrent(this->qwindow.get());
}

const std::shared_ptr<opengl::QGlContext> &GlWindow::get_context() const {
	return this->qcontext;
}

} // namespace opengl
} // namespace renderer
} // namespace openage
