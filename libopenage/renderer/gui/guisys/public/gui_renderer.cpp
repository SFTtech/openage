// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "gui_renderer.h"

#include <QSize>

#include "renderer/gui/guisys/private/gui_renderer_impl.h"

namespace qtgui {

GuiRenderer::GuiRenderer(std::shared_ptr<openage::renderer::Window> window) :
	impl{std::make_unique<GuiRendererImpl>(window)} {
}

GuiRenderer::~GuiRenderer() = default;

GLuint GuiRenderer::render() {
	return this->impl->render();
}

void GuiRenderer::resize(int w, int h) {
	this->impl->resize(QSize{w, h});
}

} // namespace qtgui
