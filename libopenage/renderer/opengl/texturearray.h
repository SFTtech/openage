// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../resources/texture_data.h"
#include "simple_object.h"


namespace openage {
namespace renderer {
namespace opengl {

/// A handle to an OpenGL texture.
class GlTextureArray final : public GlSimpleObject {
public:
	/// Constructs a texture and fills it with the given data.
	GlTextureArray(const resources::TextureData&);

	GlTextureArray(int layers,int height, int width,resources::pixel_format format);

	int submit_texture(resources::TextureData& data);
	void bind();
    private:
	int width,height;
	int depth;
	int current_layers = 0;
    resources::pixel_format format;
	resources::TextureInfo info;
};

}}} // openage::renderer::opengl
