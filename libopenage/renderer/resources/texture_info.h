// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <vector>

#include "../../gamedata/texture.gen.h"


namespace openage {
namespace renderer {
namespace resources {

/// How the pixels are represented in a texture.
enum class pixel_format {
    r16ui, //16 bits per pixel, stored as unsigned int
	/// 24 bits per pixel, no alpha channel
	rgb8,
	/// 32 bits per pixel
	rgba8,
	/// 24 bits per pixel, depth texture
	depth24,
};

/// Information for texture processing.
/// The class supports subtextures, so that one big texture ("texture atlas")
/// can contain several smaller images. These are the ones actually to be
/// rendered.
class TextureInfo {
public:
	/// Constructs a TextureInfo with the given information.
	TextureInfo(size_t width, size_t height, pixel_format, std::vector<gamedata::subtexture>&&);

	TextureInfo() = default;
	~TextureInfo() = default;

	/// @returns the format of pixels in this texture
	pixel_format get_format() const;

	/// Returns the dimensions of the whole texture bitmap
	/// @returns tuple(width, height)
	std::pair<int32_t, int32_t> get_size() const;

	/// Returns the number of available subtextures
	size_t get_subtexture_count() const;

	/// Returns the coordinates of the subtexture with the given ID
	const gamedata::subtexture& get_subtexture(size_t subid) const;

	/// Returns the size of the subtexture with the given ID
	std::pair<int32_t, int32_t> get_subtexture_size(size_t subid) const;

	/// Returns atlas subtexture coordinates.
	///
	/// @returns left, right, top and bottom bounds as coordinates these pick
	/// the requested area out of the big texture. returned as floats in
	/// range 0.0 to 1.0, relative to the whole surface size.
	std::tuple<float, float, float, float> get_subtexture_coordinates(size_t subid) const;

protected:
	/// Width and height of this texture.
	int32_t w, h;

	/// The pixel format of this texture.
	pixel_format format;

	/// Some textures are merged together into texture atlases, large images which contain
	/// more than one individual texture. These are their positions in the atlas.
	std::vector<gamedata::subtexture> subtextures;
};

}}}
