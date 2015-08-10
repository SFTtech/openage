// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "../../config.h"
#if WITH_OPENGL

#include "texture.h"

#include <epoxy/gl.h>

#include "../../error/error.h"


namespace openage {
namespace renderer {
namespace opengl {


Texture::Texture(const TextureData &txt) {
	// generate opengl texture handle
	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, id);

	int input_format, output_format;

	// select pixel format
	switch (txt.format) {
	case texture_format::rgb:
		input_format = GL_RGB8;
		output_format = GL_RGB;
		break;
	case texture_format::rgba:
		input_format = GL_RGBA8;
		output_format = GL_RGBA;
		break;
	default:
		throw Error{MSG(err) << "invalid texture format passed to OpenGL."};
	}

	// store raw pixels to gpu
	glTexImage2D(
		GL_TEXTURE_2D, 0,
		input_format, txt.w, txt.h, 0,
		output_format, GL_UNSIGNED_BYTE, txt.data.get()
	);

	// drawing settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
	glDeleteTextures(1, &this->id);
}


}}} // openage::renderer::opengl

#endif
