// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "../resources/texture_data.h"
#include "../texture.h"
#include "simple_object.h"


namespace openage {
namespace renderer {
namespace opengl {

/// A handle to an OpenGL texture.
class GlTexture2d final : public Texture2d, public GlSimpleObject {
public:
	/// Constructs a texture and fills it with the given data.
	explicit GlTexture2d(const resources::Texture2dData&);

	/// Constructs an empty texture with the given parameters.
	GlTexture2d(resources::Texture2dInfo const&);

	resources::Texture2dData into_data() override;

	void upload(resources::Texture2dData const&) override;
};

}}} // openage::renderer::opengl
