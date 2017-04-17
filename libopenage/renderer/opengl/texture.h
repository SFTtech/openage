// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <epoxy/gl.h>
#include <experimental/optional>

#include "../texture.h"


namespace openage {
namespace renderer {

namespace resources {
class TextureData;
}

namespace opengl {

/**
 * A handle to an OpenGL texture.
 */
class GlTexture : public Texture {
public:
	/// Constructs a texture and fills it with the given data.
	explicit GlTexture(const resources::TextureData&);

	/// Constructs an empty texture with the given parameters.
	GlTexture(size_t width, size_t height, resources::pixel_format);
	~GlTexture();

	// TODO support copying in the future
	GlTexture(const GlTexture&) = delete;
	GlTexture &operator =(const GlTexture&) = delete;

	/// Moving is supported.
	GlTexture(GlTexture&&);
	GlTexture &operator =(GlTexture&&);

	/// Returns the OpenGL handle to this texture.
	GLuint get_handle() const;

	resources::TextureData into_data() override;

private:
	/// The OpenGL handle to this texture.
	std::experimental::optional<GLuint> handle;

	/// The current pack alignment. See glPixelStorei
	static GLint PACK_ALIGNMENT;
	/// The current unpack alignment. See glPixelStorei
	static GLint UNPACK_ALIGNMENT;

	void adjust_unpack_alignment(const GLint alignment);
	void adjust_pack_alignment(const GLint alignment);
};

}}} // openage::renderer::opengl
