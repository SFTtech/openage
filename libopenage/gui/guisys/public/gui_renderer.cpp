// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "../public/gui_renderer.h"

#include <QSize>

#include "../private/gui_renderer_impl.h"

namespace qtsdl {

GuiRenderer::GuiRenderer(SDL_Window *window)
	:
	impl{std::make_unique<GuiRendererImpl>(window)} {
}

GuiRenderer::~GuiRenderer() {
}

void GuiRenderer::render() {
	this->impl->render();
}

void GuiRenderer::resize(int w, int h) {
	this->impl->resize(QSize{w, h});
}

} // namespace qtsdl
