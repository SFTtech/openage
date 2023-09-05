// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>


namespace openage::renderer::resources {
class Texture2dInfo;

struct blending_mask {
	char directions;
	size_t texture_id;
	size_t subtex_id;
};

/**
 * Contains information about a terrain blending pattern.
 */
class BlendPatternInfo {
public:
	/**
	 * Create a blending pattern info.
	 *
	 * @param scalefactor Factor by which sprite images are scaled down at default zoom level.
	 *                    Applies to all masks. Choosing a factor that is power of 2 is
	 *                    recommended.
	 * @param textures Information object of textures used by the pattern.
	 * @param masks Layer information.
	 */
	BlendPatternInfo(const float scalefactor,
	                 std::vector<std::shared_ptr<Texture2dInfo>> &textures,
	                 std::vector<blending_mask> &masks);

	BlendPatternInfo() = default;
	~BlendPatternInfo() = default;

	/**
	 * Get the scaling factor of the blending pattern.
	 *
	 * @return A float containing the scaling factor.
	 */
	float get_scalefactor() const;

	/**
	 * Get number of textures referenced by the blending pattern.
	 *
	 * @return Number of textures.
	 */
	size_t get_texture_count() const;

	/**
	 * Get the 2D texture information of the texture with the specified index.
	 *
	 * @param idx Index of the texture.
	 *
	 * @return A texture information object containing the metadata of the
	 *         referenced texture.
	 */
	const std::shared_ptr<Texture2dInfo> &get_texture(size_t idx) const;

	/**
	 * Get number of masks in the blending pattern.
	 *
	 * @return Number of layers.
	 */
	size_t get_mask_count() const;

	/**
	 * Get the blending masks for specified directions where two terrains overlap.
	 *
	 * @param directions Directions where the terrains overlap.
	 *
	 * @return Blending masks that match the specified directions.
	 */
	std::vector<std::pair<size_t, size_t>> get_masks(char directions) const;

private:
	/**
	 * Scaling factor of the animation across all layers at default zoom level.
	 */
	float scalefactor;

	/**
	 * Information about textures used by the animation.
	 */
	std::vector<std::shared_ptr<Texture2dInfo>> textures;

	/**
	 * Layer information.
	 */
	std::unordered_map<char, std::pair<size_t, size_t>> masks;
};

} // namespace openage::renderer::resources
