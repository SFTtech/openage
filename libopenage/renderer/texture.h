// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "resources/texture_data.h"


namespace openage {
namespace renderer {

/// An abstract base for a handle to a texture buffer allocated in graphics hardware.
/// Can be obtained by passing texture data to the renderer.
class Texture {
public:
	virtual ~Texture();

	/// Returns the texture information.
	const resources::TextureInfo& get_info() const;

	/// Copies this texture's data from graphics hardware into a CPU-accessible
	/// TextureData buffer.
	virtual resources::TextureData into_data() const = 0;

protected:
	/// Constructs the base with the given information.
	Texture(resources::TextureInfo);

	/// Information about the size, format, etc. of this texture.
	resources::TextureInfo info;
};

}}
