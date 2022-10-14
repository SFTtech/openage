// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QWindow)
QT_FORWARD_DECLARE_CLASS(QOpenGLContext)

namespace openage {
namespace renderer {
namespace opengl {

class GlShaderProgram;

/// Stores information about context capabilities and limitations.
struct qgl_context_capabilities {
	/// The maximum number of vertex attributes in a shader.
	size_t max_vertex_attributes;
	/// The amount of texture units (GL_TEXTUREi) available.
	size_t max_texture_slots;
	/// The maximum size of a single dimension of a texture.
	size_t max_texture_size;
	/// The maximum number of binding points for uniform blocks
	/// in a single shader.
	size_t max_uniform_buffer_bindings;

	int major_version;
	int minor_version;
};

// ASDF: Qt port
/// Manages an OpenGL context.
class QGlContext {
public:
	/// Create a GL context in the given Qt window.
	explicit QGlContext(const std::shared_ptr<QWindow> &);
	~QGlContext() = default;

	/// It doesn't make sense to have more than one instance of the same context.
	QGlContext(const QGlContext &) = delete;
	QGlContext &operator=(const QGlContext &) = delete;

	/// We have to support moving to avoid a mess somewhere else.
	QGlContext(QGlContext &&);
	QGlContext &operator=(QGlContext &&);

	/// Returns the underlying Qt context pointer.
	std::shared_ptr<QOpenGLContext> get_raw_context() const;

	/// Returns the capabilities of this context.
	qgl_context_capabilities get_capabilities() const;

	/// Turns VSYNC on or off for this context.
	void set_vsync(bool on);

	/// Checks whether the current GL context on this thread reported any errors
	/// and throws an exception if it did. Note that it's static.
	static void check_error();

	/// Returns a handle to the last-activated shader program
	const std::weak_ptr<GlShaderProgram> &get_current_program() const;

	/// Store the last-activated shader program
	void set_current_program(const std::shared_ptr<GlShaderProgram> &);

private:
	/// The associated Qt window is held here so the context remains active.
	std::shared_ptr<QWindow> window;

	/// Pointer to Qt struct representing the GL context.
	std::shared_ptr<QOpenGLContext> gl_context;

	/// Context capabilities.
	qgl_context_capabilities capabilities{};

	// The last-active shader program
	std::weak_ptr<GlShaderProgram> last_program;
};

} // namespace opengl
} // namespace renderer
} // namespace openage
