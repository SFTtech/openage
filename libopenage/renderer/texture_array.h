// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "resources/texture_data.h"


namespace openage {
namespace renderer {

/// A texture array, where individual elements are 2D textures. The array elements
/// are referred to as "layers", and every layer must have the same format
/// (size, pixel format, etc).
class Texture2dArray {
public:
	virtual ~Texture2dArray();

	/// Returns information about the layer format.
	resources::Texture2dInfo const& get_info() const;

	/// Uploads the given texture data into the specified layer. `layer` must
	/// be strictly less than the size of the array and the data format must
	/// match the format this array was originally created with.
	virtual void upload(size_t layer, resources::Texture2dData const&) = 0;

protected:
	/// Constructs the base class.
	Texture2dArray(const resources::Texture2dInfo&);

	/// Information about the size, format, etc. of every layer in this array.
	resources::Texture2dInfo layer_info;
};

}} // openage::renderer
