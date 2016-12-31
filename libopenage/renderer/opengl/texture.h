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
	Texture(renderer::Context *context, const TextureData &data);
	~Texture();

	/**
	 * Bind this texture to the given slot id.
	 */
	void bind_to(int slot) const override;

protected:
	/**
	 * OpenGL handle id.
	 */
	GLuint id;
};


}}} // openage::renderer::opengl

#endif
