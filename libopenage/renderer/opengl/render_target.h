// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <experimental/optional>

#include "../renderer.h"
#include "texture.h"
#include "../../error/error.h"


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

class GlRenderTarget : public RenderTarget {
public:
	/// Construct a render target pointed at the default framebuffer - the window.
	GlRenderTarget();
	~GlRenderTarget();

	/// Construct a render target pointing at the given textures.
	/// Texture are attached to points specific to their pixel format,
	/// e.g. a depth texture will be set as the depth target.
	GlRenderTarget(std::vector<const GlTexture*> textures);

	/// Cannot copy a framebuffer.
	GlRenderTarget(const GlRenderTarget&) = delete;
	GlRenderTarget &operator =(const GlRenderTarget&) = delete;

	/// Can move the object.
	GlRenderTarget(GlRenderTarget&&);
	GlRenderTarget &operator =(GlRenderTarget&&);

	/// Bind this framebuffer to GL_READ_FRAMEBUFFER.
	void bind_read() const;

	/// Bind this framebuffer to GL_DRAW_FRAMEBUFFER.
	void bind_write() const;

private:
	/// The type.
	gl_render_target_t type;

	/// The handle to the underlying OpenGL object.
	std::experimental::optional<GLuint> handle;
};

}}}
