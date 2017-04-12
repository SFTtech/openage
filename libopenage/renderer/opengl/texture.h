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
	GlTexture(const resources::TextureData&);
	~GlTexture();

	/// Returns the OpenGL handle to this texture.
	GLuint get_handle() const;

private:
	/// The OpenGL handle to this texture.
	GLuint handle;
};

}}} // openage::renderer::opengl
