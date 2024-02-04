// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

#include "datastructure/constexpr_map.h"
#include "renderer/resources/texture_subinfo.h"
#include "util/path.h"


namespace openage::renderer::resources {

/**
 * How the pixels are represented in a texture.
 */
enum class pixel_format {
	/// 16 bits per pixel, unsigned integer, single channel
	r16ui,
	/// 32 bits per pixel, unsigned integer, single channel
	r32ui,
	/// 24 bits per pixel, float, RGB order
	rgb8,
	/// 24 bits per pixel, float, BGR order
	bgr8,
	/// 24 bits per pixel, depth texture
	depth24,
	/// 32 bits per pixel, float, alpha channel, RGBA order
	rgba8,
	/// 32 bits per pixel, unsigned integer, alpha channel, RGBA order
	rgba8ui,
};

/**
 * Get the size in bytes of a single pixel of the specified format.
 *
 * @param fmt Pixel format enum value.
 *
 * @return Size of single pixel (in bytes).
 */
constexpr size_t pixel_size(pixel_format fmt) {
	constexpr auto pix_size = datastructure::create_const_map<pixel_format, size_t>(
		std::make_pair(pixel_format::r16ui, 2),
		std::make_pair(pixel_format::r32ui, 4),
		std::make_pair(pixel_format::rgb8, 3),
		std::make_pair(pixel_format::bgr8, 3),
		std::make_pair(pixel_format::rgba8, 4),
		std::make_pair(pixel_format::rgba8ui, 4),
		std::make_pair(pixel_format::depth24, 3));

	return pix_size.get(fmt);
}

/**
 * Information about a 2D texture surface, without actual texture data.
 * The class supports subtextures, so that one big texture ("texture atlas")
 * can contain several smaller images, which can be extracted for rendering
 * one-by-one.
 */
class Texture2dInfo {
public:
	/**
	 * Create a new 2D texture information.
	 *
	 * @param width Width of texture.
	 * @param height Height of texture.
	 * @param fmt Pixel format used by the texture.
	 * @param imagepath Path to the texture image (optional).
	 * @param row_alignment Byte alignment of pixels (optional).
	 * @param subs List of subtexture information (optional).
	 */
	Texture2dInfo(size_t width,
	              size_t height,
	              pixel_format fmt,
	              std::optional<util::Path> imagepath = std::nullopt,
	              size_t row_alignment = 1,
	              std::vector<Texture2dSubInfo> &&subs = std::vector<Texture2dSubInfo>{});

	Texture2dInfo() = default;
	Texture2dInfo(Texture2dInfo const &) = default;
	~Texture2dInfo() = default;

	/**
	 * Compares the texture parameters _excluding_ the subtexture information.
	 *
	 * @return true if the parameters are equal, else false.
	 */
	bool operator==(Texture2dInfo const &);

	/**
	 * Compares the texture parameters _excluding_ the subtexture information.
	 * Inverse of \p operator==(..) .
	 *
	 * @return true if the parameters are not equal, else false.
	 */
	bool operator!=(Texture2dInfo const &);

	/**
	 * Get the dimensions of the whole texture bitmap.
	 *
	 * @return Size of texture as (width, height) tuple.
	 */
	std::pair<int32_t, int32_t> get_size() const;

	/**
	 * Get the format of pixels in this texture.
	 *
	 * @return Pixel format.
	 */
	pixel_format get_format() const;

	/**
	 * Get the byte alignment of texture rows to byte boundaries.
	 *
	 * @return Size of byte alignment in bytes.
	 */
	size_t get_row_alignment() const;

	/**
	 * Get the size of a single row in bytes, including possible
	 * padding at its end.
	 *
	 * @return Row size (in bytes).
	 */
	size_t get_row_size() const;

	/**
	 * Get the size in bytes of the raw pixel data. It is equal to
	 * \p get_row_size() * \p get_size().second .
	 *
	 * @return Size of the raw pixel data (in bytes).
	 */
	size_t get_data_size() const;

	/**
	 * Get the path to the image resource of this texture.
	 *
	 * @return Image path to texture.
	 */
	const std::optional<util::Path> &get_image_path() const;

	/**
	 * Get the number of subtextures in this texture.
	 *
	 * @return Number of subtextures.
	 */
	size_t get_subtex_count() const;

	/**
	 * Get the subtexture information for a specific subtexture.
	 *
	 * @param subidx Index of the subtexture.
	 * @return Subtexture information object.
	 */
	const Texture2dSubInfo &get_subtex_info(size_t subidx) const;

	/**
	 * Get the coordinates of a specific subtexture inside the main texture.
	 * Coordinates are returned as normalized values (floats in range 0.0 to 1.0).
	 *
	 * @deprecated Use \p get_subtex_tile_params() instead.
	 *
	 * @param subidx Index of the subtexture.
	 *
	 * @return 4-tuple with normalized coordinates: (left, right, top, bottom)
	 */
	std::tuple<float, float, float, float> get_subtexture_coordinates(size_t subidx) const;

private:
	/**
	 * Width and height of this texture.
	 */
	int32_t w, h;

	/**
	 * Storage format of the pixels inside the texture.
	 */
	pixel_format format;

	/**
	 * The alignment of texture rows to byte boundaries. Can be 1, 2, 4 or 8.
	 * There is padding at the end of each row to match the alignment if the
	 * row size is not a multiple of the alignment.
	 */
	size_t row_alignment;

	/**
	 * Path to image resource.
	 */
	std::optional<util::Path> imagepath;

	/**
	 * Positions of subtextures inside the texture.
	 *
	 * Some textures are merged together into texture atlases, large images which contain
	 * more than one individual texture.
	 */
	std::vector<Texture2dSubInfo> subtextures;
};

} // namespace openage::renderer::resources
