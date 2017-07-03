// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QtGlobal>

struct SDL_Window;

QT_FORWARD_DECLARE_CLASS(QOpenGLContext)

namespace qtsdl {

class CtxExtractionMode;

class GuiRenderingCtxActivator;

/**
 * Returns a GL context usable by Qt classes.
 * Provides pre- and post-rendering functions to make the context usable for GUI rendering.
 */
class GuiRenderingSetupRoutines {
public:
	explicit GuiRenderingSetupRoutines(SDL_Window *window);
	~GuiRenderingSetupRoutines();

	QOpenGLContext* get_ctx();

private:
	friend class GuiRenderingCtxActivator;
	void pre_render();
	void post_render();

	std::unique_ptr<CtxExtractionMode> ctx_extraction_mode;
};

/**
 * Prepares the context for rendering the GUI for one frame.
 * Activator must be destroyed as soon as the GUI has executed its frame render call.
 */
class GuiRenderingCtxActivator {
public:
	explicit GuiRenderingCtxActivator(GuiRenderingSetupRoutines &rendering_setup_routines);
	~GuiRenderingCtxActivator();

	GuiRenderingCtxActivator(GuiRenderingCtxActivator&& o);
	GuiRenderingCtxActivator& operator=(GuiRenderingCtxActivator&& o);

private:
	GuiRenderingCtxActivator(const GuiRenderingCtxActivator&) = delete;
	GuiRenderingCtxActivator& operator=(const GuiRenderingCtxActivator&) = delete;

	GuiRenderingSetupRoutines *rendering_setup_routines;
};

} // namespace qtsdl
