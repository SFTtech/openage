// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <optional>

#include "renderer/opengl/framebuffer.h"
#include "renderer/render_target.h"
#include "renderer/renderer.h"


namespace openage {
namespace renderer {

class Texture2d;

namespace opengl {

class GlTexture2d;

/**
 * The type of OpenGL render target.
 */
enum class gl_render_target_t {
	/**
	 * Render into a framebuffer.
	 */
	framebuffer,
	// TODO renderbuffers mixed with textures
};

/**
 * Represents an OpenGL target that can be drawn into.
 * It can be either a framebuffer with texture attachements or the display (the window).
 */
class GlRenderTarget final : public RenderTarget {
public:
	/**
	 * Construct a render target pointed at the default framebuffer - the window.
	 *
	 * @param context OpenGL context used for drawing.
	 * @param width Current width of the window.
	 * @param height Current height of the window.
	 */
	GlRenderTarget(const std::shared_ptr<GlContext> &context,
	               size_t width,
	               size_t height);

	/**
	 * Construct a render target pointing at the given textures.
	 * Texture are attached to points specific to their pixel format,
	 * e.g. a depth texture will be set as the depth target.
	 *
	 * @param context OpenGL context used for drawing.
	 * @param textures Texture attachements.
	 */
	GlRenderTarget(const std::shared_ptr<GlContext> &context,
	               std::vector<std::shared_ptr<GlTexture2d>> const &textures);

	/**
	 * Get the pixels stored in the render target's buffer.
	 *
	 * @return Texture data with the image contents of the buffer.
	 */
	resources::Texture2dData into_data() override;

	/**
	 * Get the type of this render target.
	 *
	 * @return Render target type.
	 */
	gl_render_target_t get_type() const;

	/**
	 * Get the targeted textures.
	 *
	 * @return Textures drawn into by the render target.
	 */
	std::vector<std::shared_ptr<Texture2d>> get_texture_targets() override;

	/**
	 * Resize the render target to the specified dimensions.
	 *
	 * This is used to scale the viewport to the correct size when
	 * binding the render target with write access.
	 *
	 * @param width New width.
	 * @param height New height.
	 */
	void resize(size_t width, size_t height);

	/**
	 * Bind this render target to be drawn into.
	 */
	void bind_write() const;

	/**
	 * Bind this render target to be read from.
	 */
	void bind_read() const;

private:
	/**
	 * Type of this render target.
	 */
	gl_render_target_t type;

	/**
	 * Size of the window or the texture targets.
	 */
	std::pair<size_t, size_t> size;

	/**
	 * For framebuffer target type, the framebuffer.
	 */
	std::optional<GlFramebuffer> framebuffer;

	/**
	 * Target textures if the render target is a textured fbo.
	 */
	std::optional<std::vector<std::shared_ptr<GlTexture2d>>> textures;
};

} // namespace opengl
} // namespace renderer
} // namespace openage
