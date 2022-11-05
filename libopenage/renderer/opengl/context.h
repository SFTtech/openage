// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QWindow)
QT_FORWARD_DECLARE_CLASS(QOpenGLContext)
QT_FORWARD_DECLARE_CLASS(QOpenGLDebugLogger)

namespace openage::renderer::opengl {

class GlDebugLogHandler;
class GlShaderProgram;

/// Stores information about context capabilities and limitations.
struct gl_context_capabilities {
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

/// Manages an OpenGL context.
class GlContext {
public:
	/**
	 * Create a GL context for the given Qt window.
	 * 
	 * TODO: Currently, this also sets up the Qt window with QWindow::create() because
	 * the constructur also creates the context format. Ideally, QWindow creation should
	 * be handed to our own Window class.
	 * 
	 * @param window Window for the context. The context is made current to this window.
	*/
	explicit GlContext(const std::shared_ptr<QWindow> &window);
	~GlContext() = default;

	/**
	 * Copy this context.
	 * 
	 * It doesn't make sense to have more than one instance of the same context.
	*/
	GlContext(const GlContext &) = delete;
	GlContext &operator=(const GlContext &) = delete;

	/**
	 * Move this context.
	 * 
	 * We have to support moving to avoid a mess somewhere else.
	*/
	GlContext(GlContext &&);
	GlContext &operator=(GlContext &&);

	/**
	 * Get the underlying Qt OpenGL context object.
	 * 
	 * @return Qt's OpenGL context object.
	*/
	std::shared_ptr<QOpenGLContext> get_raw_context() const;

	/**
	 * Get the capabilities of this context.
	*/
	gl_context_capabilities get_capabilities() const;

	/**
	 * Activate or deactivate VSync for this context.
	 * 
	 * @param on Pass \p true to activate VSync, \p false to deactivate.
	*/
	void set_vsync(bool on);

	/**
	 * Check whether the current GL context on this thread reported any errors.
	 * If any OpenGL error is found, throw an exception.
	 * 
	 * @throw openage::error::Error Error with OpenGL error type.
	*/
	static void check_error();

	/**
	 * Get the currently active shader program.
	 * 
	 * If \p nullptr is returned, the current shader program is unknown. There
	 * may be an active shader loaded by external libraries or scripts.
	 * 
	 * @return Currently active shader program.
	*/
	const std::weak_ptr<GlShaderProgram> &get_current_program() const;

	/**
	 * Store the currently active shader program for this context.
	 * 
	 * Note that this method does not load the shader in OpenGL. It is merely
	 * a conveniance function so that the renderer can check which program
	 * is currently used.
	 * 
	 * @param prog Currently active shader program.
	*/
	void set_current_program(const std::shared_ptr<GlShaderProgram> &prog);

private:
	/**
	 * Associated Qt window. Held here so the context remains active.
	*/
	std::shared_ptr<QWindow> window;

	/**
	 * Logger for OpenGL debug messages.
	 * 
	 * TODO: Make debug logging optional
	*/
	std::shared_ptr<GlDebugLogHandler> log_handler;

	/**
	 * Pointer to Qt struct representing the GL context.
	*/
	std::shared_ptr<QOpenGLContext> gl_context;

	/**
	 * Context capabilities, i.e. available OpenGL features and version.
	*/
	gl_context_capabilities capabilities{};

	/**
	 * The last-active shader program
	*/
	std::weak_ptr<GlShaderProgram> last_program;
};

} // namespace openage::renderer::opengl
