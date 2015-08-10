// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_OPENGL_TEXTURE_H_
#define OPENAGE_RENDERER_OPENGL_TEXTURE_H_

#include <epoxy/gl.h>

#include "../texture.h"

namespace openage {
namespace renderer {
namespace opengl {

/**
 * An OpenGL texture.
 */
class Texture : public renderer::Texture {
public:
	Texture(const TextureData &data);
	~Texture();

protected:
	/**
	 * OpenGL handle id.
	 */
	GLuint id;
};


}}} // openage::renderer::opengl

#endif
