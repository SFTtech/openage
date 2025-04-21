// Copyright 2015-2025 the openage authors. See copying.md for legal info.

#include "texture.h"

#include <epoxy/gl.h>

#include <algorithm>
#include <tuple>

#include "../../datastructure/constexpr_map.h"
#include "../../error/error.h"
#include "../../log/log.h"

#include "../resources/texture_data.h"
#include "lookup.h"
#include "render_target.h"


namespace openage {
namespace renderer {
namespace opengl {

GlTexture2d::GlTexture2d(const std::shared_ptr<GlContext> &context,
                         const resources::Texture2dData &data) :
	Texture2d(data.get_info()),
	GlSimpleObject(context,
                   [](GLuint handle) { glDeleteTextures(1, &handle); }) {
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
		GL_TEXTURE_2D,
		0,
		std::get<0>(fmt_in_out),
		size.first,
		size.second,
		0,
		std::get<1>(fmt_in_out),
		std::get<2>(fmt_in_out),
		data.get_data());

	// drawing settings
	// TODO these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	log::log(MSG(dbg) << "Created OpenGL texture from data (size: "
	                  << size.first << "x" << size.second << ")");
}

GlTexture2d::GlTexture2d(const std::shared_ptr<GlContext> &context,
                         const resources::Texture2dInfo &info) :
	Texture2d(info),
	GlSimpleObject(context,
                   [](GLuint handle) { glDeleteTextures(1, &handle); }) {
	GLuint handle;
	glGenTextures(1, &handle);
	this->handle = handle;

	glBindTexture(GL_TEXTURE_2D, *this->handle);

	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->info.get_format());

	auto size = this->info.get_size();

	glPixelStorei(GL_UNPACK_ALIGNMENT, this->info.get_row_alignment());

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		std::get<0>(fmt_in_out),
		size.first,
		size.second,
		0,
		std::get<1>(fmt_in_out),
		std::get<2>(fmt_in_out),
		nullptr);

	// TODO: these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	log::log(MSG(dbg) << "Created OpenGL texture from info parameters (size: "
	                  << size.first << "x" << size.second << ")");
}

void GlTexture2d::resize(size_t width, size_t height) {
	auto prev_size = this->info.get_size();
	if (width == static_cast<size_t>(prev_size.first)
	    and height == static_cast<size_t>(prev_size.second)) {
		// size is the same, no need to resize
		log::log(MSG(dbg) << "Texture resize called, but size is unchanged (size: "
		                  << prev_size.first << "x" << prev_size.second << ")");
		return;
	}

	// only allow resizing for internal textures that are not created from
	// image files
	// TODO: maybe allow this for all textures?
	if (this->info.get_image_path().has_value()) {
		throw Error(MSG(err) << "Cannot resize a texture that was created from an image file.");
	}
	if (this->info.get_subtex_count() != 0) {
		throw Error(MSG(err) << "Cannot resize a texture that has subtextures.");
	}

	// create new info object
	this->info = resources::Texture2dInfo(width,
	                                      height,
	                                      this->info.get_format(),
	                                      this->info.get_image_path(),
	                                      this->info.get_row_alignment());

	glBindTexture(GL_TEXTURE_2D, *this->handle);

	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->info.get_format());
	auto size = this->info.get_size();

	// redefine the texture with the new size
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		std::get<0>(fmt_in_out),
		size.first,
		size.second,
		0,
		std::get<1>(fmt_in_out),
		std::get<2>(fmt_in_out),
		nullptr);

	// copy the old texture data into the new texture
	glCopyTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0,
		0,
		0,
		0,
		std::min(size.first, prev_size.first), // avoid buffer overread with std::min
		std::min(size.second, prev_size.second));

	log::log(MSG(dbg) << "Resized OpenGL texture (size: "
	                  << width << "x" << height << ")");
}

resources::Texture2dData GlTexture2d::into_data() {
	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->info.get_format());
	std::vector<uint8_t> data(this->info.get_data_size());

	glPixelStorei(GL_PACK_ALIGNMENT, this->info.get_row_alignment());
	glBindTexture(GL_TEXTURE_2D, *this->handle);
	// TODO use a Pixel Buffer Object instead
	glGetTexImage(GL_TEXTURE_2D, 0, std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), data.data());

	return resources::Texture2dData(resources::Texture2dInfo(this->info), std::move(data));
}

void GlTexture2d::upload(resources::Texture2dData const &data) {
	if (this->info != data.get_info()) {
		throw Error(MSG(err) << "Tried to upload texture data of different format into an existing GPU texture.");
	}

	glBindTexture(GL_TEXTURE_2D, *this->handle);

	auto size = this->info.get_size();
	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->info.get_format());

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.first, size.second, std::get<1>(fmt_in_out), std::get<2>(fmt_in_out), data.get_data());
}

} // namespace opengl
} // namespace renderer
} // namespace openage
