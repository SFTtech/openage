// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/texture.h"
#include <GL/gl.h>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

#include <QObject>
#include <QOffscreenSurface>
#include <QQuickRenderControl>
#include <QQuickWindow>
#include <QtGlobal>

#include "renderer/gui/guisys/private/gui_ctx_setup.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLFramebufferObject)

namespace openage::renderer {
class Window;
} // namespace openage::renderer

namespace qtgui {

class GuiRenderer;


/**
 * Passes the native graphic context to Qt.
 */
class GuiRendererImpl final : public QObject {
	Q_OBJECT

public:
	explicit GuiRendererImpl(std::shared_ptr<openage::renderer::Window> window);
	~GuiRendererImpl() = default;

	static GuiRendererImpl *impl(GuiRenderer *renderer);

	/**
	 * draw the gui into our texture.
	 */
	void render();

	/**
	 * Adjust the target window's geometry.
	 *
	 * @param width Target width.
	 * @param height Target height.
	 */
	void resize(const size_t width, const size_t height);

	QQuickWindow *get_window();

	/**
	 * update the texture where the gui shall be rendered into.
	 */
	void set_texture(const std::shared_ptr<openage::renderer::Texture2d> &texture);

signals:
	/**
	 * emitted when the off-screen gui window was resized through `resize`.
	 */
	void resized(const QSize &size);

private:
	/**
	 * Object for sending render command to Qt.
	 */
	QQuickRenderControl render_control;

	/**
	 * Qt Window that represents the scene graph of the GUI.
	 *
	 * This window is never shown onscreen, it's just used by QQuickRenderControl
	 * to control the rendering process. It draws into a texture
	 * that we pass it via QQuickWindow::setRenderTarget().
	 */
	std::unique_ptr<QQuickWindow> target_window;

	/**
	 * Texture that is targeted by the gui renderer.
	 */
	std::shared_ptr<openage::renderer::Texture2d> texture;

	/**
	 * Rendering context.
	 * Beware: in the member order, put _after_ all context-affine resources (e.g. texture).
	 * otherwise the context will be deleted before the resource in it.
	 */
	GlGuiRenderingContext gui_context;
};

} // namespace qtgui
