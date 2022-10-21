// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "simple_object.h"
#include "texture.h"


namespace openage {
namespace renderer {
namespace opengl {

/// Represents an OpenGL Framebuffer Object.
/// It is a collection of bitmap targets that can be drawn into
/// and read from.
class GlFramebuffer final : public GlSimpleObject {
public:
	/// Construct a framebuffer pointing at the given textures.
	/// Texture are attached to points specific to their pixel format,
	/// e.g. a depth texture will be set as the depth target.
	GlFramebuffer(const std::shared_ptr<QGlContext> &context,
	              std::vector<std::shared_ptr<GlTexture2d>> const &textures);

	/// Bind this framebuffer to GL_READ_FRAMEBUFFER.
	void bind_read() const;

	/// Bind this framebuffer to GL_DRAW_FRAMEBUFFER.
	void bind_write() const;
};

} // namespace opengl
} // namespace renderer
} // namespace openage
