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

private:
	/**
	 * OpenGL handle id.
	 */
	GLuint id;
};

}}} // openage::renderer::opengl
