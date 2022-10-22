// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "gui_ctx_setup.h"

#include <cassert>

#include <QOpenGLDebugLogger>
#include <QOpenGLFramebufferObject>

#include "gui/guisys/private/platforms/context_extraction.h"
#include "renderer/gui/guisys/private/opengl_debug_logger.h"
#include "renderer/window.h"
#include "renderer/opengl/window.h"
#include "renderer/opengl/context_qt.h"

namespace qtgui {

CtxExtractionException::CtxExtractionException(const std::string &what_arg) :
	std::runtime_error{what_arg} {
}

const std::shared_ptr<QOpenGLContext> &CtxExtractionMode::get_ctx() const {
	return this->ctx;
}

GlGuiRenderingContext::GlGuiRenderingContext(std::shared_ptr<openage::renderer::Window> window) :
	CtxExtractionMode{},
	offscreen_surface{} {
	//auto format = window->get_qt_window()->format();
	//this->ctx.setFormat(format);
	//this->ctx.create();
	//assert(this->ctx.isValid());

	this->ctx = std::dynamic_pointer_cast<openage::renderer::opengl::GlWindow>(window)->get_context()->get_raw_context();

	auto context_debug_parameters = get_current_opengl_debug_parameters(*this->ctx);

	this->offscreen_surface.setFormat(this->ctx->format());
	this->offscreen_surface.create();

	apply_opengl_debug_parameters(context_debug_parameters, *this->ctx);
}

void GlGuiRenderingContext::pre_render() {
	if (!this->ctx->makeCurrent(&this->offscreen_surface)) {
		assert(false);
		return;
	}
}

void GlGuiRenderingContext::post_render() {
	QOpenGLFramebufferObject::bindDefault();
	this->ctx->functions()->glFlush();
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
