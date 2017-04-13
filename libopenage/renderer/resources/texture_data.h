// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "texture_info.h"
#include "../../util/path.h"


namespace openage {
namespace renderer {
namespace resources {

class TextureData {
public:
	/**
	 * Create a texture from an image file.
	 * @param[in] use_metafile determines whether the loading should read an accompanying
	 *                         metadata file to split the texture into subtextures
	 *
	 * Uses SDL Image internally. For supported image file types,
	 * see the SDL_Image initialization in the engine.
	 */
	TextureData(const util::Path &path, bool use_metafile = false);

	TextureData(TextureInfo &&info, std::vector<uint8_t> &&data);

	const TextureInfo& get_info() const;

	const uint8_t *get_data() const;

	void store(const util::Path& file) const;

private:
	TextureInfo info;
	std::vector<uint8_t> data;

};

}}} // namespace openage::renderer::resources
