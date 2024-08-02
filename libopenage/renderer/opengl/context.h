// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QWindow)
QT_FORWARD_DECLARE_CLASS(QOpenGLContext)

namespace openage::renderer::opengl {

class GlDebugLogHandler;
class GlShaderProgram;

/**
 * Stores information about context capabilities and limitations.
 */
struct gl_context_spec {
	/// The maximum number of vertex attributes in a shader.
	size_t max_vertex_attributes;
	/// The amount of texture units (GL_TEXTUREi) available.
	size_t max_texture_slots;
	/// The maximum size of a single dimension of a texture.
	size_t max_texture_size;
	/// The maximum number of uniform locations per shader.
	size_t max_uniform_locations;
	/// The maximum number of binding points for uniform blocks
	/// in a single shader.
	size_t max_uniform_buffer_bindings;

	int major_version;
	int minor_version;
};

/**
 * Manages an OpenGL context.
 */
class GlContext {
public:
	/**
	 * Create a GL context for the given Qt window.
	 *
	 * @param window Window for the context. The context is made current to this window.
	 * @param debug If true, enable OpenGL debug logging.
	 */
	explicit GlContext(const std::shared_ptr<QWindow> &window,
	                   bool debug = false);
	~GlContext() = default;

	// It doesn't make sense to have more than one instance of the same context.
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
	 * Get the ID of the default framebuffer used for displaying to
	 * the window.
	 *
	 * This value may change on every frame, so it should be called every
	 * time the default framebuffer is bound.
	 *
	 * @return ID of the default (display) framebuffer.
	 */
	unsigned int get_default_framebuffer_id();

	/**
	 * Get the capabilities of this context.
	 */
	gl_context_spec get_specs() const;

	/**
	 * Activate or deactivate VSync for this context.
	 *
	 * TODO: This currently does not work at runtime. vsync must be set before
	 * the QWindow is created.
	 *
	 * @param on \p true to activate VSync, \p false to deactivate.
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

	/**
	 * Get a free uniform buffer binding point that is not bound to any buffer.
	 *
	 * The number of available binding points is limited by the OpenGL implementation.
	 * When the context is created, there are \p capabilities.max_uniform_buffer_bindings
	 * free binding points available.
	 *
	 * @return Binding point ID.
	 *
	 * @throw Error if no binding point is available.
	 */
	size_t get_uniform_buffer_binding();

	/**
	 * Free a buffer binding point, indicating that newly created buffers can use it.
	 *
	 * When calling this function, it must be ensured that the binding point is not
	 * assigned to any buffer or shader. Otherwise, reassigning the binding point
	 * can corrupt the uniform data.
	 *
	 * @param binding_point Binding point ID.
	 *
	 * @throw Error if the binding point is not valid.
	 */
	void free_uniform_buffer_binding(size_t binding_point);

	/**
	 * Find out the supported graphics functions and OpenGL version of the device.
	 */
	static gl_context_spec find_spec();

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
	gl_context_spec specs{};

	/**
	 * The last-active shader program
	 */
	std::weak_ptr<GlShaderProgram> last_program;

	/**
	 * Store the currently active binding points for uniform buffers.
	 */
	std::vector<bool> uniform_buffer_bindings;
};

} // namespace openage::renderer::opengl
