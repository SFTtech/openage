// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <epoxy/gl.h>

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

	/// Returns the OpenGL handle to this texture.
	GLuint get_handle() const;

	resources::TextureData into_data() const override;

private:
	/// The OpenGL handle to this texture.
	GLuint handle;
};

}}} // openage::renderer::opengl
