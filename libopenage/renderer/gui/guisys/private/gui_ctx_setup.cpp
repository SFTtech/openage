// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#include "gui_ctx_setup.h"

#include <cassert>

#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QOpenGLFramebufferObject>
#include <QWindow>

#include "renderer/gui/guisys/private/opengl_debug_logger.h"
#include "renderer/opengl/context.h"
#include "renderer/opengl/window.h"
#include "renderer/window.h"

namespace qtgui {

CtxExtractionException::CtxExtractionException(const std::string &what_arg) :
	std::runtime_error{what_arg} {
}

const std::shared_ptr<QOpenGLContext> &CtxExtractionMode::get_ctx() const {
	return this->ctx;
}

GlGuiRenderingContext::GlGuiRenderingContext(std::shared_ptr<openage::renderer::Window> window) :
	CtxExtractionMode{},
	offscreen_surface{},
	window{window} {
	auto win = std::dynamic_pointer_cast<openage::renderer::opengl::GlWindow>(this->window);
	auto window_ctx = win->get_context()->get_raw_context();
	this->ctx = std::make_shared<QOpenGLContext>();
	this->ctx->setShareContext(window_ctx.get());
	this->ctx->setFormat(window->get_qt_window()->requestedFormat());
	this->ctx->create();

	this->offscreen_surface.setFormat(this->ctx->format());
	this->offscreen_surface.create();
}

void GlGuiRenderingContext::pre_render() {
	// Make the share context current, so Qt draw into its window.
	if (!this->ctx->makeCurrent(&this->offscreen_surface)) {
		assert(false);
		return;
	}

	// Our renderer saves the current shader program loaded in OpenGL
	// to prevent unnecessary reloads. However, Qt loads its own shaders
	// when the GUI is rendered, so any shaders present in our own render
	// passes have to be reloaded. We do this by setting the current program to nullptr.
	auto win = std::dynamic_pointer_cast<openage::renderer::opengl::GlWindow>(this->window);
	auto window_ctx = win->get_context();
	window_ctx->set_current_program(nullptr);
}

void GlGuiRenderingContext::post_render() {
	QOpenGLFramebufferObject::bindDefault();
	this->ctx->functions()->glFlush();

	// switch back to the window context, so our own renderer can continue drawing
	this->ctx->doneCurrent();
	auto window_ctx = std::dynamic_pointer_cast<openage::renderer::opengl::GlWindow>(this->window)->get_context()->get_raw_context();
	if (!window_ctx->makeCurrent(this->window->get_qt_window().get())) {
		assert(false);
		return;
	}
}

QQuickGraphicsDevice GlGuiRenderingContext::get_device() {
	return QQuickGraphicsDevice::fromOpenGLContext(this->ctx.get());
}

GuiUniqueRenderingContext::GuiUniqueRenderingContext(std::shared_ptr<openage::renderer::Window> window) :
	CtxExtractionMode{} {
	auto format = window->get_qt_window()->format();
	this->ctx->setFormat(format);
	this->ctx->create();
	assert(this->ctx->isValid());

	std::shared_ptr<QWindow> w = window->get_qt_window();

	if (this->ctx->makeCurrent(w.get())) {
		return;
	}

	throw CtxExtractionException("adding GUI to the main rendering context failed");
}

void GuiUniqueRenderingContext::pre_render() {
}

void GuiUniqueRenderingContext::post_render() {
}

QQuickGraphicsDevice GuiUniqueRenderingContext::get_device() {
	return QQuickGraphicsDevice::fromOpenGLContext(this->ctx.get());
}

GuiSeparateRenderingContext::GuiSeparateRenderingContext() :
	CtxExtractionMode{} {
	this->main_ctx.create();
	assert(this->main_ctx.isValid());

	auto context_debug_parameters = get_current_opengl_debug_parameters(this->main_ctx);

	this->ctx->setFormat(this->main_ctx.format());
	this->ctx->setShareContext(&this->main_ctx);
	this->ctx->create();
	assert(this->ctx->isValid());
	assert(!(this->main_ctx.format().options() ^ this->ctx->format().options()).testFlag(QSurfaceFormat::DebugContext));

	this->offscreen_surface.setFormat(this->ctx->format());
	this->offscreen_surface.create();

	this->pre_render();
	apply_opengl_debug_parameters(context_debug_parameters, *this->ctx);
}

GuiSeparateRenderingContext::~GuiSeparateRenderingContext() {
	this->pre_render();
	this->ctx_logger.reset();
	this->post_render();
}

void GuiSeparateRenderingContext::pre_render() {
	if (!this->ctx->makeCurrent(&this->offscreen_surface)) {
		assert(false);
		return;
	}
}

void GuiSeparateRenderingContext::post_render() {
	this->make_current_back();
}

QQuickGraphicsDevice GuiSeparateRenderingContext::get_device() {
	return QQuickGraphicsDevice::fromOpenGLContext(this->ctx.get());
}

} // namespace qtgui
