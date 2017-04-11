// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "../../gamedata/texture.gen.h"


namespace openage {
namespace renderer {
namespace resources {

/// How the pixels are represented in a texture.
enum class pixel_format {
	/// 24 bits per pixel, no alpha channel
	rgb8,
	/// 32 bits per pixel
	rgba8,
};

/**
 * Information for texture processing.
 * The class supports subtextures, so that one big texture ("texture atlas")
 * can contain several smaller images. These are the ones actually to be
 * rendered.
 */
class TextureInfo {
	friend class TextureData;
public:
	pixel_format get_format() const;

	/**
	 * Return the dimensions of the whole texture bitmap
	 * @returns tuple(width, height)
	 */
	std::pair<int32_t, int32_t> get_size() const;

	/**
	 * @return the number of available subtextures
	 */
	size_t get_subtexture_count() const;

	/**
	 * Get the subtexture coordinates by its idea.
	 */
	const gamedata::subtexture& get_subtexture(size_t subid) const;

	/**
	 * Fetch the size of the given subtexture.
	 * @param subid: index of the requested subtexture
	 */
	std::pair<int32_t, int32_t> get_subtexture_size(size_t subid) const;

	/**
	 * get atlas subtexture coordinates.
	 *
	 * @returns left, right, top and bottom bounds as coordinates these pick
	 * the requested area out of the big texture. returned as floats in
	 * range 0.0 to 1.0, relative to the whole surface size.
	 */
	std::tuple<float, float, float, float> get_subtexture_coordinates(size_t subid) const;

protected:
	/// The pixel format of this texture.
	pixel_format format;

	/// Width and height of this texture.
	int32_t w, h;

	/**
	 * Some textures are merged together into texture atlases, large images which contain
	 * more than one individual texture. These are their positions in the atlas.
	 */
	std::vector<gamedata::subtexture> subtextures;
};

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
	TextureData(const char *filename, bool use_metafile = false);

	const TextureInfo& get_info() const;

	const uint8_t *get_data() const;

private:
	TextureInfo info;
	std::vector<uint8_t> data;

};

}}} // namespace openage::renderer::resources
