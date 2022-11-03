// Copyright 2015-2019 the openage authors. See copying.md for legal info.

// epoxy needs to be loaded before qt stuff
#include "renderer/opengl/texture.h"

#include "gui_renderer_impl.h"

#include "renderer/texture.h"
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


GuiRendererImpl::GuiRendererImpl(std::shared_ptr<openage::renderer::Window> window) :
	QObject{},
	gui_context{window},
	render_control{} {
	// use of undocumented as of qt6.4 constructor
	// QQuickWindow(QQuickRenderControl *renderControl)
	// which associates the render controller with a qwindow.
	this->target_window = std::make_unique<QQuickWindow>(&this->render_control);

	// same format as main window
	this->target_window->setFormat(window->get_qt_window()->format());

	// make everything that's not part of the GUI transparent
	this->target_window->setColor(Qt::transparent);

	auto size = window->get_qt_window()->size();
	this->resize(size.width(), size.height());

	this->gui_context.pre_render();
	// TODO: switch for vulkan support
	this->target_window->setGraphicsDevice(QQuickGraphicsDevice::fromOpenGLContext(this->gui_context.get_ctx().get()));
	this->render_control.initialize();

	this->gui_context.post_render();
}


void GuiRendererImpl::set_texture(const std::shared_ptr<openage::renderer::Texture2d> &texture) {
	this->gui_context.pre_render();
	this->texture = texture;
	auto tex_info = texture->get_info();
	QSize tex_size{tex_info.get_size().first, tex_info.get_size().second};

	// TODO switch here for vulkan surface draw support.
	auto tex_gl = std::dynamic_pointer_cast<openage::renderer::opengl::GlTexture2d>(texture);
	auto tex_gl_id = tex_gl->get_handle();
	this->target_window->setRenderTarget(QQuickRenderTarget::fromOpenGLTexture(tex_gl_id, tex_size));
	this->gui_context.post_render();
}


void GuiRendererImpl::render() {
	this->gui_context.pre_render();

	this->render_control.beginFrame();
	this->render_control.polishItems();
	this->render_control.sync();
	this->render_control.render();
	this->render_control.endFrame();

	this->gui_context.post_render();
}

std::shared_ptr<QQuickWindow> GuiRendererImpl::get_window() {
	return this->target_window;
}

GuiRendererImpl *GuiRendererImpl::impl(GuiRenderer *renderer) {
	return renderer->impl.get();
}

void GuiRendererImpl::resize(const size_t width, const size_t height) {
	this->gui_context.pre_render();
	this->target_window->setGeometry(0, 0, width, height);
	emit this->resized(QSize(width, height));
	this->gui_context.post_render();
}


} // namespace qtgui
