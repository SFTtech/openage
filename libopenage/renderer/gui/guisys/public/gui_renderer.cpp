// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "gui_renderer.h"

#include <QSize>

#include "renderer/gui/guisys/private/gui_renderer_impl.h"

namespace qtgui {

GuiRenderer::GuiRenderer(const std::shared_ptr<openage::renderer::Window> &window) :
	impl{std::make_unique<GuiRendererImpl>(window)} {
}

GuiRenderer::~GuiRenderer() = default;

void GuiRenderer::render() {
	this->impl->render();
}

void GuiRenderer::resize(int w, int h) {
	this->impl->resize(w, h);
}

void GuiRenderer::set_texture(const std::shared_ptr<openage::renderer::Texture2d> &texture) {
	this->impl->set_texture(texture);
}

std::shared_ptr<QQuickWindow> GuiRenderer::get_window() {
	return this->impl->get_window();
}

} // namespace qtgui
