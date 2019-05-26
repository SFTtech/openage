// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "texture.h"

#include <epoxy/gl.h>

#include <tuple>

#include "../../error/error.h"
#include "../../datastructure/constexpr_map.h"
#include "../../log/log.h"

#include "../resources/texture_data.h"
#include "render_target.h"
#include "lookup.h"


namespace openage {
namespace renderer {
namespace opengl {

GlTexture2d::GlTexture2d(const resources::Texture2dData& data)
	: Texture2d(data.get_info())
	, GlSimpleObject([] (GLuint handle) { glDeleteTextures(1, &handle); } )
{
	GLuint handle;
	glGenTextures(1, &handle);
	this->handle = handle;

	glBindTexture(GL_TEXTURE_2D, *this->handle);

	// select pixel format
	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->info.get_format());

	// store raw pixels to gpu
	auto size = this->info.get_size();

	glPixelStorei(GL_UNPACK_ALIGNMENT, this->info.get_row_alignment());

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		std::get<0>(fmt_in_out), size.first, size.second, 0,
		std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), data.get_data()
	);

	// drawing settings
	// \todo these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	log::log(MSG(dbg) << "Created OpenGL texture from data");
}

GlTexture2d::GlTexture2d(const resources::Texture2dInfo &info)
	: Texture2d(info)
	, GlSimpleObject([] (GLuint handle) { glDeleteTextures(1, &handle); } )
{
	GLuint handle;
	glGenTextures(1, &handle);
	this->handle = handle;

	glBindTexture(GL_TEXTURE_2D, *this->handle);

	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->info.get_format());

	auto dims = this->info.get_size();

	glPixelStorei(GL_UNPACK_ALIGNMENT, this->info.get_row_alignment());

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		std::get<0>(fmt_in_out), dims.first, dims.second, 0,
		std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), nullptr
	);

	// \todo these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	log::log(MSG(dbg) << "Created OpenGL texture from parameters");
}

resources::Texture2dData GlTexture2d::into_data() {
	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->info.get_format());
	std::vector<uint8_t> data(this->info.get_data_size());

	glPixelStorei(GL_PACK_ALIGNMENT, this->info.get_row_alignment());
	glBindTexture(GL_TEXTURE_2D, *this->handle);
	// \todo use a Pixel Buffer Object instead
	glGetTexImage(GL_TEXTURE_2D, 0, std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), data.data());

	return resources::Texture2dData(resources::Texture2dInfo(this->info), std::move(data));
}

void GlTexture2d::upload(resources::Texture2dData const& data) {
	if (this->info != data.get_info()) {
		throw Error(MSG(err) << "Tried to upload texture data of different format into an existing GPU texture.");
	}

	glBindTexture(GL_TEXTURE_2D, *this->handle);

	auto size = this->info.get_size();
	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->info.get_format());

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
	                size.first, size.second,
	                std::get<1>(fmt_in_out), std::get<2>(fmt_in_out),
	                data.get_data()
	);
}

}}} // openage::renderer::opengl
