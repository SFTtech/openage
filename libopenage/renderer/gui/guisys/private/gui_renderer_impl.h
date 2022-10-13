// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

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

class EventHandlingQuickWindow : public QQuickWindow {
	Q_OBJECT

public:
	explicit EventHandlingQuickWindow(QQuickRenderControl *render_control);
	virtual ~EventHandlingQuickWindow();

public slots:
	void on_input_event(std::atomic<bool> *processed, QEvent *event, bool only_if_grabbed);
	void on_resized(const QSize &size);

private:
	// TODO: to remove when the proper focus for the foreign (that obtained from QWindow::fromWinId()) windows is implemented (Qt 5.6).
	QQuickItem *focused_item;
};

/**
 * Passes the native graphic context to Qt.
 */
class GuiRendererImpl : public QObject {
	Q_OBJECT

public:
	explicit GuiRendererImpl(std::shared_ptr<openage::renderer::Window> window);
	~GuiRendererImpl();

	static GuiRendererImpl *impl(GuiRenderer *renderer);

	/**
	 * @return texture ID where GUI was rendered
	 */
	GLuint render();

	/**
	 * Adjust the target window's geometry.
	 *
	 * @param width Target width.
	 * @param height Target height.
	 */
	void resize(const size_t width, const size_t height);
	void resize(const QSize &size);

	QQuickWindow *get_window();

signals:
	void resized(const QSize &size);

private slots:
	/**
	 * ASDF: Qt port.
	 * Create the GUI texture that QT writes into.
	 */
	void create_texture();

	/**
	 * ASDF: Qt port.
	 * Destroy the GUI texture that QT writes into.
	 */
	void destroy_texture();

private:
	// ASDF: Qt port
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
	 * Rendering context.
	 */
	GlGuiRenderingContext gui_context;

	/**
	 * ID of the texture that is targeted by the gui renderer.
	 *
	 * TODO: Use renderer::Texture2d class.
	 */
	GLuint target_texture_id;
};

class TemporaryDisableGuiRendererSync {
public:
	explicit TemporaryDisableGuiRendererSync(GuiRendererImpl &renderer);
	~TemporaryDisableGuiRendererSync();

private:
	TemporaryDisableGuiRendererSync(const TemporaryDisableGuiRendererSync &) = delete;
	TemporaryDisableGuiRendererSync &operator=(const TemporaryDisableGuiRendererSync &) = delete;

	GuiRendererImpl &renderer;
	const bool need_sync;
};

} // namespace qtgui

// ASDF: Qt5
// Q_DECLARE_METATYPE(std::atomic<bool>*)
