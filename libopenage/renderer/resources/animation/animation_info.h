// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "renderer/resources/animation/layer_info.h"
#include "util/vector.h"


namespace openage::renderer::resources {
class Texture2dInfo;
class Texture2dSubInfo;

/**
 * Contains information about a 2D animation. The animation data can be
 * stored in several spritesheet textures, although it is usually only one.
 *
 * Animations are composed of layers which are animated independently from
 * each other. Layers each contain angle definitions which contain the frame
 * information.
 */
class Animation2dInfo {
public:
	/**
	 * Create a 2D Animation Info.
	 *
	 * @param scalefactor Factor by which sprite images are scaled down at default zoom level.
	 *                    Applies to all layers. Choosing a factor that is power of 2 is
	 *                    recommended.
	 * @param textures Information object of textures used by the animation.
	 * @param layers Layer information.
	 */
	Animation2dInfo(const float scalefactor,
	                std::vector<std::shared_ptr<Texture2dInfo>> &textures,
	                std::vector<LayerInfo> &layers);

	Animation2dInfo() = default;
	~Animation2dInfo() = default;

	/**
	 * Get the scaling factor of the animation.
	 *
	 * @return A float containing the scaling factor.
	 */
	float get_scalefactor() const;

	/**
	 * Get number of textures referenced by the animation.
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
	 * Get number of layers in the animation.
	 *
	 * @return Number of layers.
	 */
	size_t get_layer_count() const;

	/**
	 * Get the layer information of the layer with the specified index.
	 *
	 * @param idx Index of the layer.
	 *
	 * @return A layer information object.
	 */
	const LayerInfo &get_layer(size_t idx) const;

	/**
	 * Get the maximum boundaries for all frames in the animation.
	 *
	 * This represents the maximum distance from the anchor point to the
	 * left, right, top, bottom edges of the frames (in pixels).
	 *
	 * @return Boundaries of the animation (in pixels): [left, right, top, bottom]
	 */
	const util::Vector4i &get_max_bounds() const;

	/**
	 * Get the maximum size for all frames in the animation.
	 *
	 * This represents the maximum width and height of the frames (in pixels).
	 *
	 * @return Size of the animation (in pixels): [width, height]
	 */
	const util::Vector2s get_max_size() const;

private:
	/**
	 * Scaling factor of the animation across all layers at default zoom level.
	 */
	float scalefactor;

	/**
	 * Information about textures used by the animation.
	 */
	std::vector<std::shared_ptr<Texture2dInfo>> texture_infos;

	/**
	 * Layer information.
	 */
	std::vector<LayerInfo> layers;

	/**
	 * Maximum boundaries for all frames in the animation.
	 *
	 * This represents the maximum distance from the anchor point to the
	 * left, right, top, bottom edges of the frames (in pixels).
	 */
	util::Vector4i max_bounds;
};

} // namespace openage::renderer::resources
