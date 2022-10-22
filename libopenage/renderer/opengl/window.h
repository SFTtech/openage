// Copyright 2018-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "../window.h"

#include <optional>


QT_FORWARD_DECLARE_CLASS(QWindow)
QT_FORWARD_DECLARE_CLASS(QOpenGLContext)

namespace qtsdl {
class GuiApplication;
}

namespace openage {
namespace renderer {

class EventHandlingQuickWindow;

namespace opengl {

class GlContext;
class QGlContext;

class GlWindow final : public Window {
public:
	/// Create a shiny window with the given title.
	GlWindow(const std::string &title, size_t width, size_t height);
	~GlWindow() = default;

	void set_size(size_t width, size_t height) override;

	void update() override;

	std::shared_ptr<Renderer> make_renderer() override;

	/// Make this window's context the current rendering context of the current thread.
	/// Only use this and most other GL functions on a dedicated window thread.
	void make_context_current();

	/// Return a pointer to this window's GL context.
	const std::shared_ptr<opengl::QGlContext> &get_context() const;

private:
	// OpenGL Context in Qt
	std::shared_ptr<opengl::QGlContext> context;
};

} // namespace opengl
} // namespace renderer
} // namespace openage
