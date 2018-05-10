// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "texture.h"

#include <epoxy/gl.h>

#include <tuple>

#include "../../error/error.h"
#include "../../datastructure/constexpr_map.h"
#include "../../log/log.h"

#include "../resources/texture_data.h"
#include "render_target.h"


namespace openage {
namespace renderer {
namespace opengl {

/// The input and output formats for GL.
static constexpr auto gl_format = datastructure::create_const_map<resources::pixel_format, std::tuple<GLint, GLenum, GLenum>>(
	// TODO check correctness of formats here
	std::make_pair(resources::pixel_format::r16ui, std::make_tuple(GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_INT)),
	std::make_pair(resources::pixel_format::r32ui, std::make_tuple(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT)),
	std::make_pair(resources::pixel_format::rgb8, std::make_tuple(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::bgr8, std::make_tuple(GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::rgba8, std::make_tuple(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::rgba8ui, std::make_tuple(GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE)),
	std::make_pair(resources::pixel_format::depth24, std::make_tuple(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE))
);

GlTexture::GlTexture(const resources::TextureData& data)
	: Texture(data.get_info())
	, GlSimpleObject([] (GLuint handle) { glDeleteTextures(1, &handle); } )
{
	// generate opengl texture handle
	glGenTextures(1, &*this->handle);
	glBindTexture(GL_TEXTURE_2D, *this->handle);

	// select pixel format
	auto fmt_in_out = gl_format.get(this->info.get_format());

	// store raw pixels to gpu
	auto size = this->info.get_size();

	glPixelStorei(GL_UNPACK_ALIGNMENT, this->info.get_row_alignment());

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		std::get<0>(fmt_in_out), size.first, size.second, 0,
		std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), data.get_data()
	);

	// drawing settings
	// TODO these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	log::log(MSG(dbg) << "Created OpenGL texture from data");
}

GlTexture::GlTexture(const resources::TextureInfo &info)
	: Texture(info)
	, GlSimpleObject([] (GLuint handle) { glDeleteTextures(1, &handle); } )
{
	// generate opengl texture handle
	glGenTextures(1, &*this->handle);
	glBindTexture(GL_TEXTURE_2D, *this->handle);

	auto fmt_in_out = gl_format.get(this->info.get_format());

	auto dims = this->info.get_size();

	glPixelStorei(GL_UNPACK_ALIGNMENT, this->info.get_row_alignment());

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		std::get<0>(fmt_in_out), dims.first, dims.second, 0,
		std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), nullptr
	);

	// TODO these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	log::log(MSG(dbg) << "Created OpenGL texture from parameters");
}

resources::TextureData GlTexture::into_data() {
	auto fmt_in_out = gl_format.get(this->info.get_format());
	std::vector<uint8_t> data(this->info.get_data_size());

	glPixelStorei(GL_PACK_ALIGNMENT, this->info.get_row_alignment());
	glBindTexture(GL_TEXTURE_2D, *this->handle);
	// TODO use a Pixel Buffer Object instead
	glGetTexImage(GL_TEXTURE_2D, 0, std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), data.data());

	return resources::TextureData(resources::TextureInfo(this->info), std::move(data));
}

void GlTexture::bind(){
	glBindTexture(GL_TEXTURE_2D, *this->handle);
}

}}} // openage::renderer::opengl
