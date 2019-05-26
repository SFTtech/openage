// Copyright 2018-2019 the openage authors. See copying.md for legal info.

#include "texture_array.h"

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

GlTexture2dArray::GlTexture2dArray(const std::vector<resources::Texture2dData>& data)
	// Call the data-less constructor first.
	: GlTexture2dArray(data.size(), data[0].get_info())
{
	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->layer_info.get_format());
	auto size = this->layer_info.get_size();

	// Fill the array with given data
	size_t i = 0;
	for (auto const& tex : data) {
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		                0,                          // mipmap number
		                0, 0, i,                    // xoffset, yoffset, zoffset
		                size.first, size.second, 1, // width, height, depth
		                std::get<1>(fmt_in_out),    // format
		                std::get<2>(fmt_in_out),    // type
		                tex.get_data()              // data
		);

		i += 1;
	}
}

GlTexture2dArray::GlTexture2dArray(size_t n_layers, resources::Texture2dInfo const& layer_info)
	: Texture2dArray(layer_info)
	, GlSimpleObject([] (GLuint handle) { glDeleteTextures(1, &handle); } )
	, n_layers(n_layers)
{
	GLuint handle;
	glGenTextures(1, &handle);
	this->handle = handle;

	glBindTexture(GL_TEXTURE_2D_ARRAY, *this->handle);

	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->layer_info.get_format());
	auto size = this->layer_info.get_size();

	// Create empty image
	glTexImage3D(GL_TEXTURE_2D_ARRAY,
	             0,                       // mipmap level
	             std::get<0>(fmt_in_out), // gpu texel format
	             size.first,              // width
	             size.second,             // height
	             n_layers,                // depth
	             0,                       // border
	             std::get<1>(fmt_in_out), // cpu pixel format
	             std::get<2>(fmt_in_out), // cpu pixel type
	             nullptr                  // data
	);

	// \todo these are outdated, use sampler settings
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	log::log(MSG(dbg) << "Created an OpenGL texture array.");
}

void GlTexture2dArray::upload(size_t layer, resources::Texture2dData const& data) {
	if (layer >= this->n_layers) {
		throw Error(MSG(err) << "Cannot upload to layer " << layer << " in texture array with "
		                     << this->n_layers << " layers.");
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, *this->handle);

	auto fmt_in_out = GL_PIXEL_FORMAT.get(this->layer_info.get_format());
	auto size = this->layer_info.get_size();

	glTexSubImage3D( GL_TEXTURE_2D_ARRAY,
	                 0,                          // mipmap number
	                 0,0,layer,                  // xoffset, yoffset, zoffset
	                 size.first, size.second, 1, // width, height, depth
	                 std::get<1>(fmt_in_out),    // format
	                 std::get<2>(fmt_in_out),    // type
	                 data.get_data()             // data
	);
}

}}} // openage::renderer::opengl
