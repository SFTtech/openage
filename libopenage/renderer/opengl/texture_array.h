// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "../texture_array.h"

#include "simple_object.h"
#include "../resources/texture_data.h"


namespace openage {
namespace renderer {
namespace opengl {

/// An OpenGL array of 2D textures.
class GlTexture2dArray final : public Texture2dArray, public GlSimpleObject {
public:
	/// Constructs an array of the same number of layers as the size of the given
	/// vector, and fills the layers with the corresponding vector element. The
	/// texture formats in all vector elements must be the same as defined by
	/// Textur2dInfo::operator==.
	GlTexture2dArray(const std::vector<resources::Texture2dData>&);

	/// Constructs an array of ln_layers empty layers, with the per-layer texture
	/// format specified in layer_info.
	GlTexture2dArray(size_t n_layers, resources::Texture2dInfo layer_info);

	void upload(size_t layer, resources::Texture2dData const&) override;

private:
	/// The number of layers (elements) in the array, AKA the depth.
	size_t n_layers;
};

}}} // openage::renderer::opengl
