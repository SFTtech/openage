// Copyright 2018-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "../window.h"

#include <optional>

#include "context.h"

QT_FORWARD_DECLARE_CLASS(QWindow)
QT_FORWARD_DECLARE_CLASS(QOpenGLContext)

namespace qtsdl {
class GuiApplication;
}

namespace openage {
namespace renderer {
namespace opengl {

class GlWindow final : public Window {
public:
	/// Create a shiny window with the given title.
	GlWindow(const std::string &title, size_t width, size_t height);
	~GlWindow() = default;

	std::shared_ptr<SDL_Window> get_sdl_window() override;
	std::shared_ptr<QWindow> get_qt_window() override;

	void set_size(size_t width, size_t height) override;

	void update() override;

	std::shared_ptr<Renderer> make_renderer() override;

	/// Make this window's context the current rendering context of the current thread.
	/// Only use this and most other GL functions on a dedicated window thread.
	void make_context_current();

	/// Return a pointer to this window's GL context.
	const std::shared_ptr<opengl::GlContext> &get_context() const;

private:
	/// The SDL window to which the OpenGL context is associated.
	std::shared_ptr<SDL_Window> window;

	/// The window's OpenGL context. It can't be constructed immediately,
	/// but after the constructor runs it's guaranteed to be available.
	/// The SDL window is also held within this context, because when the window is deallocated,
	/// the context is gone.
	std::shared_ptr<opengl::GlContext> context;

	/// The Qt window to which the OpenGL context is associated.
	std::shared_ptr<QWindow> qwindow;

	// OpenGL Context in Qt
	std::shared_ptr<QOpenGLContext> qcontext;
};

} // namespace opengl
} // namespace renderer
} // namespace openage
