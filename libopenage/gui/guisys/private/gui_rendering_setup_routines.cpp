// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "gui_rendering_setup_routines.h"

#include <cassert>

#include <QDebug>

#include "gui_ctx_setup.h"

namespace qtsdl {

GuiRenderingSetupRoutines::GuiRenderingSetupRoutines(SDL_Window *window) {
	try {
		this->ctx_extraction_mode = std::make_unique<GuiUniqueRenderingContext>(window);
	} catch (const CtxExtractionException&) {

		qInfo() << "Falling back to separate render context for GUI";

		try {
			this->ctx_extraction_mode = std::make_unique<GuiSeparateRenderingContext>(window);
		} catch (const CtxExtractionException&) {
			assert(false && "setting up context for GUI failed");
		}
	}
}

GuiRenderingSetupRoutines::~GuiRenderingSetupRoutines() {
}

QOpenGLContext* GuiRenderingSetupRoutines::get_ctx() {
	return this->ctx_extraction_mode->get_ctx();
}

void GuiRenderingSetupRoutines::pre_render() {
	this->ctx_extraction_mode->pre_render();
}

void GuiRenderingSetupRoutines::post_render() {
	this->ctx_extraction_mode->post_render();
}

GuiRenderingCtxActivator::GuiRenderingCtxActivator(GuiRenderingSetupRoutines &rendering_setup_routines)
	:
	rendering_setup_routines{&rendering_setup_routines} {

	this->rendering_setup_routines->pre_render();
}

GuiRenderingCtxActivator::~GuiRenderingCtxActivator() {
	if (this->rendering_setup_routines)
		this->rendering_setup_routines->post_render();
}

GuiRenderingCtxActivator::GuiRenderingCtxActivator(GuiRenderingCtxActivator&& o)
	:
	rendering_setup_routines{o.rendering_setup_routines} {

	o.rendering_setup_routines = nullptr;
}

GuiRenderingCtxActivator& GuiRenderingCtxActivator::operator=(GuiRenderingCtxActivator&& o) {
	this->rendering_setup_routines = o.rendering_setup_routines;
	o.rendering_setup_routines = nullptr;
	return *this;
}

} // namespace qtsdl
