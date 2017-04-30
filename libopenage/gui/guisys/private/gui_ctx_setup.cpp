// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "gui_ctx_setup.h"

#include <cassert>

#include <QOpenGLDebugLogger>

#include "platforms/context_extraction.h"
#include "opengl_debug_logger.h"

namespace qtsdl {

CtxExtractionException::CtxExtractionException(const std::string &what_arg)
	:
	std::runtime_error{what_arg} {
}

QOpenGLContext* CtxExtractionMode::get_ctx() {
	return &this->ctx;
}

GuiUniqueRenderingContext::GuiUniqueRenderingContext(SDL_Window *window)
	:
	CtxExtractionMode{} {

	QVariant handle;
	WId id;

	std::tie(handle, id) = extract_native_context(window);

	if (handle.isValid()) {
		// pass the SDL opengl context so qt can use it
		this->ctx.setNativeHandle(handle);
		this->ctx.create();
		assert(this->ctx.isValid());

		// reuse the sdl window
		QWindow *w = QWindow::fromWinId(id);
		w->setSurfaceType(QSurface::OpenGLSurface);

		if (this->ctx.makeCurrent(w)) {
			return;
		}
	}

	throw CtxExtractionException("adding GUI to the main rendering context failed");
}

void GuiUniqueRenderingContext::pre_render() {
}

void GuiUniqueRenderingContext::post_render() {
}

GuiSeparateRenderingContext::GuiSeparateRenderingContext(SDL_Window *window)
	:
	CtxExtractionMode{} {

	QVariant handle;

	std::tie(handle, this->make_current_back) = extract_native_context_and_switchback_func(window);

	if (handle.isValid()) {
		this->main_ctx.setNativeHandle(handle);
		this->main_ctx.create();
		assert(this->main_ctx.isValid());

		auto context_debug_parameters = get_current_opengl_debug_parameters(this->main_ctx);

		this->ctx.setFormat(this->main_ctx.format());
		this->ctx.setShareContext(&this->main_ctx);
		this->ctx.create();
		assert(this->ctx.isValid());
		assert(!(this->main_ctx.format().options() ^ this->ctx.format().options()).testFlag(QSurfaceFormat::DebugContext));

		this->offscreen_surface.setFormat(this->ctx.format());
		this->offscreen_surface.create();

		this->pre_render();
		apply_opengl_debug_parameters(context_debug_parameters, this->ctx);
		this->post_render();
	} else {
		throw CtxExtractionException("creating separate context for GUI failed");
	}
}

GuiSeparateRenderingContext::~GuiSeparateRenderingContext() {
	this->pre_render();
	this->ctx_logger.reset();
	this->post_render();
}

void GuiSeparateRenderingContext::pre_render() {
	if (!this->ctx.makeCurrent(&this->offscreen_surface)) {
		assert(false);
		return;
	}
}

void GuiSeparateRenderingContext::post_render() {
	this->make_current_back();
}

} // namespace qtsdl
