// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "texture.h"

#include <epoxy/gl.h>

#include "../../error/error.h"
#include "../resources/texture_data.h"


namespace openage {
namespace renderer {
namespace opengl {

GlTexture::GlTexture(const resources::TextureData& data)
	: Texture(data.get_info()) {
	// generate opengl texture handle
	glGenTextures(1, &this->handle);
	glBindTexture(GL_TEXTURE_2D, this->handle);

	GLint input_format;
	GLenum output_format;

	// select pixel format
	switch (this->info.get_format()) {
	case resources::pixel_format::rgb8:
		input_format = GL_RGB8;
		output_format = GL_RGB;
		break;
	case resources::pixel_format::rgba8:
		input_format = GL_RGBA8;
		output_format = GL_RGBA;
		break;
	default:
		throw Error{MSG(err) << "invalid texture format passed to OpenGL."};
	}

	// store raw pixels to gpu
	auto size = this->info.get_size();
	glTexImage2D(
		GL_TEXTURE_2D, 0,
		input_format, size.first, size.second, 0,
		output_format, GL_UNSIGNED_BYTE, data.get_data()
	);

	// drawing settings
	// TODO these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

GlTexture::~GlTexture() {
	glDeleteTextures(1, &this->handle);
}

GLuint GlTexture::get_handle() const {
	return this->handle;
}

}}} // openage::renderer::opengl
