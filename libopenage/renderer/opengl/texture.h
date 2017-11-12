// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../resources/texture_data.h"
#include "../texture.h"
#include "simple_object.h"


namespace openage {
namespace renderer {
namespace opengl {

/// A handle to an OpenGL texture.
class GlTexture final : public Texture, public GlSimpleObject {
public:
	/// Constructs a texture and fills it with the given data.
	explicit GlTexture(const resources::TextureData&);

	/// Constructs an empty texture with the given parameters.
	GlTexture(resources::TextureInfo const&);

	/// Downloads the texture from the GPU and returns it as CPU-accessible data.
	resources::TextureData into_data() override;
};

}}} // openage::renderer::opengl
