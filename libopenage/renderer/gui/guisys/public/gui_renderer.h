// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QQuickWindow)

namespace openage::renderer {
class Texture2d;
class Window;
} // namespace openage::renderer

namespace qtgui {

class GuiRendererImpl;

/**
 * Passes the native graphic context to Qt.
 */
class GuiRenderer {
public:
	explicit GuiRenderer(const std::shared_ptr<openage::renderer::Window> &window);
	~GuiRenderer();

	void render();
	void resize(int w, int h);
	void set_texture(const std::shared_ptr<openage::renderer::Texture2d> &texture);
	std::shared_ptr<QQuickWindow> get_window();

private:
	friend class GuiRendererImpl;
	std::unique_ptr<GuiRendererImpl> impl;
};

} // namespace qtgui
