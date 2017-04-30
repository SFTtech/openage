// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <stdexcept>
#include <memory>
#include <functional>

#include <QOpenGLContext>
#include <QOffscreenSurface>

struct SDL_Window;

QT_FORWARD_DECLARE_CLASS(QOpenGLDebugLogger)

namespace qtsdl {

class CtxExtractionException : public std::runtime_error {
public:
	explicit CtxExtractionException(const std::string &what_arg);
};

/**
 * Abstract base for the method of getting a Qt-usable context.
 */
class CtxExtractionMode {
public:
	virtual ~CtxExtractionMode() {
	}

	/**
	 * @return context that can be used by Qt
	 */
	QOpenGLContext* get_ctx();

	/**
	 * Function that must be called before rendering the GUI.
	 */
	virtual void pre_render() = 0;

	/**
	 * Function that must be called after rendering the GUI.
	 */
	virtual void post_render() = 0;

protected:
	QOpenGLContext ctx;
};

/**
 * Use the same context to render the GUI.
 */
class GuiUniqueRenderingContext : public CtxExtractionMode {
public:
	explicit GuiUniqueRenderingContext(SDL_Window *window);

	virtual void pre_render() override;
	virtual void post_render() override;
};

/**
 * Create a separate context to render the GUI, make it shared with the main context.
 */
class GuiSeparateRenderingContext : public CtxExtractionMode {
public:
	explicit GuiSeparateRenderingContext(SDL_Window *window);
	virtual ~GuiSeparateRenderingContext();

	virtual void pre_render() override;
	virtual void post_render() override;

private:
	/**
	 * GL context of the game
	 */
	QOpenGLContext main_ctx;

	/**
	 * GL debug logger of the GL context of the GUI
	 */
	std::unique_ptr<QOpenGLDebugLogger> ctx_logger;

	/**
	 * Function to make the game context current
	 */
	std::function<void()> make_current_back;

	/**
	 * Surface that is needed to make the GUI context current
	 */
	QOffscreenSurface offscreen_surface;
};

} // namespace qtsdl
