// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "texture.h"

#include <epoxy/gl.h>
#include <tuple>

#include "../../error/error.h"
#include "../resources/texture_data.h"
#include "render_target.h"


namespace openage {
namespace renderer {
namespace opengl {

/// Returns the input and output formats for GL.
inline static std::tuple<GLint, GLenum, GLenum> gl_format(resources::pixel_format fmt) {
	switch (fmt) {
	case resources::pixel_format::r16ui:
		return std::make_tuple(GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT);
	case resources::pixel_format::rgb8:
		return std::make_tuple(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
	case resources::pixel_format::rgba8:
		return std::make_tuple(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	default:
		throw Error(MSG(err) << "invalid texture format passed to OpenGL.");
	}
}

static GLint alignment_requirement(resources::pixel_format fmt) {
	switch (fmt) {
	case resources::pixel_format::r16ui:
		return 2;
	case resources::pixel_format::rgb8:
		return 1;
	case resources::pixel_format::rgba8:
		return 4;
	default:
		throw Error(MSG(err) << "invalid texture format passed to OpenGL.");
	} 
}

GLint GlTexture::PACK_ALIGNMENT = 4;
GLint GlTexture::UNPACK_ALIGNMENT = 4;

GlTexture::GlTexture(const resources::TextureData& data)
	: Texture(data.get_info())
{
	// generate opengl texture handle
	glGenTextures(1, &*this->handle);
	glBindTexture(GL_TEXTURE_2D, *this->handle);

	// select pixel format
	auto fmt_in_out = gl_format(this->info.get_format());

	// store raw pixels to gpu
	auto size = this->info.get_size();
	
	adjust_unpack_alignment(alignment_requirement(this->info.get_format()));

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		std::get<0>(fmt_in_out), size.first, size.second, 0,
		std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), data.get_data()
	);

	// drawing settings
	// TODO these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

GlTexture::GlTexture(size_t width, size_t height, resources::pixel_format fmt)
	: Texture(resources::TextureInfo(width,
	                                 height,
	                                 fmt,
	                                 std::vector<gamedata::subtexture>()))
{
	// generate opengl texture handle
	glGenTextures(1, &*this->handle);
	glBindTexture(GL_TEXTURE_2D, *this->handle);

	auto fmt_in_out = gl_format(fmt);

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		std::get<0>(fmt_in_out), width, height, 0,
		std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), nullptr
	);

	// TODO these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

GlTexture::GlTexture(GlTexture &&other)
	: Texture(std::move(other.info))
	, handle(std::move(other.handle)) {
	// make the other handle empty
	other.handle = std::experimental::optional<GLuint>();
}

GlTexture &GlTexture::operator =(GlTexture &&other) {
	if (this->handle) {
		glDeleteTextures(1, &*this->handle);
	}

	this->info = std::move(other.info);
	this->handle = std::move(other.handle);
	// make the other handle empty
	other.handle = std::experimental::optional<GLuint>();

	return *this;
}

GlTexture::~GlTexture() {
	if (this->handle) {
		glDeleteTextures(1, &*this->handle);
	}
}

GLuint GlTexture::get_handle() const {
	return *this->handle;
}

inline static size_t pixel_size(resources::pixel_format fmt) {
	switch (fmt) {
	case resources::pixel_format::r16ui:
		return 2;
	case resources::pixel_format::rgb8:
		return 3;
	case resources::pixel_format::rgba8:
		return 4;
	default:
		throw Error(MSG(err) << "Tried to find size of unknown pixel format.");
	}
}

resources::TextureData GlTexture::into_data() {
	auto size = this->info.get_size();
	auto fmt_in_out = gl_format(this->info.get_format());
	std::vector<uint8_t> data(size.first * size.second * pixel_size(this->info.get_format()));

	adjust_pack_alignment(alignment_requirement(this->info.get_format()));
	glBindTexture(GL_TEXTURE_2D, *this->handle);
	glGetTexImage(GL_TEXTURE_2D, 0, std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), data.data());

	return resources::TextureData(resources::TextureInfo(this->info), std::move(data));
}

void GlTexture::adjust_unpack_alignment(GLint alignment) {
	if (alignment == this->UNPACK_ALIGNMENT) {
		return;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
	this->UNPACK_ALIGNMENT = alignment;
}

void GlTexture::adjust_pack_alignment(GLint alignment) {
	if (alignment == this->PACK_ALIGNMENT) {
		return;
	}

	glPixelStorei(GL_PACK_ALIGNMENT, alignment);
	this->PACK_ALIGNMENT = alignment;
}

}}} // openage::renderer::opengl
