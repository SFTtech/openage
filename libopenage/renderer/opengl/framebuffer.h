// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "renderer/opengl/simple_object.h"


namespace openage::renderer::opengl {

class GlTexture2d;

/**
 * The type of OpenGL framebuffer.
 */
enum class gl_framebuffer_t {
	/**
	 * The actual window. This is visible to the user after swapping front and back buffers.
	 */
	display,
	/**
	 * A bunch of textures. These can be color texture, depth textures, etc.
	 */
	textures,
};

/**
 * Represents an OpenGL Framebuffer Object.
 * It is a collection of bitmap targets that can be drawn into
 * and read from.
 */
class GlFramebuffer final : public GlSimpleObject {
public:
	/**
	 * Construct a framebuffer pointing at the default framebuffer - the window.
	 *
	 * Drawing into this framebuffer draws onto the screen.
	 *
	 * @param context OpenGL context used for drawing.
	 */
	GlFramebuffer(const std::shared_ptr<GlContext> &context);

	/**
	 * Construct a framebuffer pointing at the given textures.
	 *
	 * Texture are attached to points specific to their pixel format,
	 * e.g. a depth texture will be set as the depth target.
	 *
	 * @param context OpenGL context used for drawing.
	 * @param textures Textures targeted by the framebuffer. They are automatically
	 *                 attached to the correct attachement points depending on their type.
	 */
	GlFramebuffer(const std::shared_ptr<GlContext> &context,
	              std::vector<std::shared_ptr<GlTexture2d>> const &textures);

	/**
	 * Get the type of this framebuffer.
	 *
	 * @return Framebuffer type.
	 */
	gl_framebuffer_t get_type() const;

	/**
	 * Bind this framebuffer to \p GL_READ_FRAMEBUFFER.
	 */
	void bind_read() const;

	/**
	 * Bind this framebuffer to \p GL_DRAW_FRAMEBUFFER.
	 */
	void bind_write() const;

private:
	/**
	 * Type of this framebuffer.
	 */
	gl_framebuffer_t type;
};

} // namespace openage::renderer::opengl
