// Copyright 2018-2024 the openage authors. See copying.md for legal info.

#include "window.h"

#include <QGuiApplication>
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QString>

#include "error/error.h"
#include "log/log.h"

#include "renderer/opengl/context.h"
#include "renderer/opengl/renderer.h"
#include "renderer/window_event_handler.h"


namespace openage::renderer::opengl {

GlWindow::GlWindow(const std::string &title,
                   window_settings settings) :
	Window{settings.width, settings.height} {
	if (QGuiApplication::instance() == nullptr) {
		// Qt windows need to attach to a QtGuiApplication
		throw Error{MSG(err) << "Failed to create Qt window: QGuiApplication has not been created yet."};
	}

	QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
	this->window = std::make_shared<QWindow>();

	this->window->setTitle(QString::fromStdString(title));
	this->window->resize(settings.width, settings.height);

	this->window->setSurfaceType(QSurface::OpenGLSurface);

	auto gl_specs = GlContext::find_spec();
	QSurfaceFormat format{};
	format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
	format.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);

	if (not settings.vsync) {
		format.setSwapInterval(0);
	}

	format.setMajorVersion(gl_specs.major_version);
	format.setMinorVersion(gl_specs.minor_version);

	format.setAlphaBufferSize(8);
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);

	if (settings.debug) {
		format.setOption(QSurfaceFormat::DebugContext);
	}

	// TODO: Set format as default for all windows with QSurface::setDefaultFormat()
	this->window->setFormat(format);
	this->window->create();

	// set display mode
	// Reset to a known state
	this->window->setWindowState(Qt::WindowNoState);
	switch (settings.mode) {
	case window_mode::WINDOWED:
		// nothing to do because it's the default
		break;
	case window_mode::BORDERLESS:
		this->window->setFlags(this->window->flags() | Qt::FramelessWindowHint);
		break;
	case window_mode::FULLSCREEN:
		this->window->setWindowState(Qt::WindowFullScreen);
		break;
	default:
		throw Error{MSG(err) << "Invalid window mode."};
	}

	this->context = std::make_shared<GlContext>(this->window, settings.debug);
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
		case QEvent::MouseButtonDblClick: {
			auto const ev = std::dynamic_pointer_cast<QMouseEvent>(event);
			for (auto &cb : this->on_mouse_button) {
				cb(*ev);
			}
		} break;
		case QEvent::MouseMove: {
			auto const ev = std::dynamic_pointer_cast<QMouseEvent>(event);
			for (auto &cb : this->on_mouse_move) {
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

	this->add_resize_callback([renderer](size_t w, size_t h, double scale) {
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
