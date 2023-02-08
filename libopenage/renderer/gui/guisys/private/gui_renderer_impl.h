// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/texture.h"

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
 * Renderer for drawing a Qt QML GUI into an openage texture.
 *
 * The method ued in this class closely follows the QQuickRenderControl flow
 * provided by Qt. Essentially, we tell Qt to render its GUI into an offscreen
 * surface (our texture). See
 * https://doc.qt.io/qt-6/qquickrendercontrol.html#details
 * for more info.
 */
class GuiRendererImpl final : public QObject {
	Q_OBJECT

public:
	explicit GuiRendererImpl(std::shared_ptr<openage::renderer::Window> window);
	~GuiRendererImpl() = default;

	static GuiRendererImpl *impl(GuiRenderer *renderer);

	/**
	 * Draw the QML GUI into the assigned GUI texture.
	 */
	void render();

	/**
	 * Adjust the offscreen surface's geometry.
	 *
	 * @param width Target width.
	 * @param height Target height.
	 */
	void resize(const size_t width, const size_t height);

	/**
	 * Set the texture that the GUI is rendered into.
	 */
	void set_texture(const std::shared_ptr<openage::renderer::Texture2d> &texture);

	/**
	 * Get the offscreen window.
	 */
	std::shared_ptr<QQuickWindow> get_window();

signals:
	/**
	 * Emitted when the off-screen gui window was resized through `resize`.
	 */
	void resized(const QSize &size);

private:
	/**
	 * Rendering context.
	 *
	 * Beware: in the member order, put _before_ all context-affine resources (e.g. texture).
	 * otherwise the context will be deleted before the resource in it.
	 */
	GlGuiRenderingContext gui_context;

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
	std::shared_ptr<QQuickWindow> target_window;

	/**
	 * GUI texture that is targeted by the render control.
	 */
	std::shared_ptr<openage::renderer::Texture2d> texture;
};

} // namespace qtgui
