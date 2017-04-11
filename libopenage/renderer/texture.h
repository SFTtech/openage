// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <SDL2/SDL_image.h>

#include <memory>
#include <tuple>
#include <vector>

#include "../gamedata/texture.gen.h"
#include "resources/texture_data.h"


namespace openage {
namespace renderer {

/**
 * A handle to texture data that has been uploaded to the GPU.
 * Obtained by registering some texture data to the renderer.
 */
class Texture {
protected:
	Texture(resources::TextureInfo);

public:
	virtual ~Texture();

	const resources::TextureInfo& get_info() const;

protected:
	resources::TextureInfo info;
};

}}
