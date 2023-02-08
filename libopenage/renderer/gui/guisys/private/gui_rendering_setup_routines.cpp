// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#include "gui_rendering_setup_routines.h"

#include <cassert>

#include <QDebug>

#include "renderer/gui/guisys/private/gui_ctx_setup.h"

namespace qtgui {

GuiRenderingSetupRoutines::GuiRenderingSetupRoutines(std::shared_ptr<openage::renderer::Window> window) {
	this->ctx_extraction_mode = std::make_unique<GlGuiRenderingContext>(window);
	// try {
	// 	this->ctx_extraction_mode = std::make_unique<GuiUniqueRenderingContext>(window);
	// }
	// catch (const CtxExtractionException &) {
	// 	qInfo() << "Falling back to separate render context for GUI";

	// 	try {
	// 		this->ctx_extraction_mode = std::make_unique<GuiSeparateRenderingContext>();
	// 	}
	// 	catch (const CtxExtractionException &) {
	// 		assert(false && "setting up context for GUI failed");
	// 	}
	// }
}

GuiRenderingSetupRoutines::~GuiRenderingSetupRoutines() = default;

const std::shared_ptr<QOpenGLContext> &GuiRenderingSetupRoutines::get_ctx() const {
	return this->ctx_extraction_mode->get_ctx();
}

void GuiRenderingSetupRoutines::pre_render() {
	this->ctx_extraction_mode->pre_render();
}

void GuiRenderingSetupRoutines::post_render() {
	this->ctx_extraction_mode->post_render();
}

GuiRenderingCtxActivator::GuiRenderingCtxActivator(GuiRenderingSetupRoutines &rendering_setup_routines) :
	rendering_setup_routines{&rendering_setup_routines} {
	this->rendering_setup_routines->pre_render();
}

GuiRenderingCtxActivator::~GuiRenderingCtxActivator() {
	if (this->rendering_setup_routines)
		this->rendering_setup_routines->post_render();
}

GuiRenderingCtxActivator::GuiRenderingCtxActivator(GuiRenderingCtxActivator &&o) :
	rendering_setup_routines{o.rendering_setup_routines} {
	o.rendering_setup_routines = nullptr;
}

GuiRenderingCtxActivator &GuiRenderingCtxActivator::operator=(GuiRenderingCtxActivator &&o) {
	this->rendering_setup_routines = o.rendering_setup_routines;
	o.rendering_setup_routines = nullptr;
	return *this;
}

} // namespace qtgui
