// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <optional>

#include "../renderer.h"
#include "texture.h"
#include "framebuffer.h"


namespace openage {
namespace renderer {
namespace opengl {

/// The type of OpenGL render target
enum class gl_render_target_t {
	/// The actual window. This is visible to the user after swapping front and back buffers
	display,
	/// A bunch of textures
	textures,
	// TODO renderbuffers mixed with textures
};

/// Represents an OpenGL target that can be drawn into.
/// It can be either a framebuffer or the display (the window).
class GlRenderTarget final : public RenderTarget {
public:
	/// Construct a render target pointed at the default framebuffer - the window.
	GlRenderTarget();

	/// Construct a render target pointing at the given textures.
	/// Texture are attached to points specific to their pixel format,
	/// e.g. a depth texture will be set as the depth target.
	GlRenderTarget(const std::shared_ptr<GlContext> &context,
	               std::vector<std::shared_ptr<GlTexture2d>> const& textures);

	/// Bind this render target to be drawn into.
	void bind_write() const;

	/// Bind this render target to be read from.
	void bind_read() const;

private:
	gl_render_target_t type;

	/// For textures target type, the framebuffer.
	std::optional<GlFramebuffer> framebuffer;
};

}}} // openage::renderer::opengl
