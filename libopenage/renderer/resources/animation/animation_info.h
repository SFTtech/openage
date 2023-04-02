// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "renderer/resources/animation/layer_info.h"
#include "renderer/resources/texture_info.h"

namespace openage::renderer {

class Texture2d;

namespace resources {

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
};

} // namespace resources
} // namespace openage::renderer
