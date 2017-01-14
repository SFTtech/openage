// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <experimental/string_view>

#include "../../gamedata/texture.gen.h"


namespace openage {
namespace renderer {
namespace resources {

/// How the pixels are represented in the texture.
enum class pixel_format {
	/// 24 bits per pixel, no alpha channel
	rgb8,
	/// 32 bits per pixel
	rgba8,
};

/// A resource containing texture data.
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
	static TextureData load_from_file(std::experimental::string_view filename, bool use_metafile=false);

private:
	/// Doesn't make sense to initialize texture data with nothing.
	TextureData() = delete;

	/// Create a texture by copying the data from a C-style buffer.
	TextureData(uint32_t width, uint32_t height, pixel_format, uint8_t *data, std::vector<gamedata::subtexture>&&);

	/// Create a texture by moving the data.
	TextureData(uint32_t width, uint32_t height, pixel_format, std::vector<uint8_t> &&data, std::vector<gamedata::subtexture>&&);

private:
	/// The pixel format of this texture.
	pixel_format format;

	/// Width and height of this texture.
	uint32_t w, h;

	/// Raw texture pixel data.
	std::vector<uint8_t> data;

	/**
	 * Some textures are merged together into texture atlases, large images which contain
	 * more than one individual texture. These are their positions in the atlas.
	 */
	std::vector<gamedata::subtexture> subtextures;
};

}}} // namespace openage::renderer::resources
