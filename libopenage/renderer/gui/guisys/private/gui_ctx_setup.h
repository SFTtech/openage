// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <stdexcept>

#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QQuickGraphicsDevice>

QT_FORWARD_DECLARE_CLASS(QOpenGLDebugLogger)

namespace openage::renderer {
class Window;
}

namespace qtgui {

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
	const std::shared_ptr<QOpenGLContext> &get_ctx() const;

	/**
	 * Function that must be called before rendering the GUI.
	 */
	virtual void pre_render() = 0;

	/**
	 * Function that must be called after rendering the GUI.
	 */
	virtual void post_render() = 0;

	/**
	 * Get a device object associated with the context.
	 */
	virtual QQuickGraphicsDevice get_device() = 0;

protected:
	/**
	 * OpenGL Context for drawing. Should be acquired from the
	 * displayed openage window.
	 */
	std::shared_ptr<QOpenGLContext> ctx;
};

/**
 * OpenGL Context manager for rendering the GUI.
 */
class GlGuiRenderingContext : public CtxExtractionMode {
public:
	explicit GlGuiRenderingContext(std::shared_ptr<openage::renderer::Window> window);

	/**
	 * Makes the OpenGL context current to the offscreen surface.
	 */
	virtual void pre_render() override;

	/**
	 * Indicate to Qt that OpenGL drawing is finished.
	 */
	virtual void post_render() override;

	/**
	 * Get a Qt device object associated with the context.
	 *
	 * @return Device object of the OpenGL context.
	 */
	virtual QQuickGraphicsDevice get_device() override;

private:
	/**
	 * Surface that is needed to make the GUI context current.
	 */
	QOffscreenSurface offscreen_surface;

	/**
	 * TODO: Actual window (for switching back context).
	 */
	std::shared_ptr<openage::renderer::Window> window;
};

/**
 * Use the same context to render the GUI.
 */
class GuiUniqueRenderingContext : public CtxExtractionMode {
public:
	explicit GuiUniqueRenderingContext(std::shared_ptr<openage::renderer::Window> window);

	virtual void pre_render() override;
	virtual void post_render() override;

	/**
	 * Get a device object associated with the context.
	 */
	virtual QQuickGraphicsDevice get_device() override;
};

/**
 * Create a separate context to render the GUI, make it shared with the main context.
 */
class GuiSeparateRenderingContext : public CtxExtractionMode {
public:
	explicit GuiSeparateRenderingContext();
	virtual ~GuiSeparateRenderingContext();

	virtual void pre_render() override;
	virtual void post_render() override;

	/**
	 * Get a device object associated with the context.
	 */
	virtual QQuickGraphicsDevice get_device() override;

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

} // namespace qtgui
