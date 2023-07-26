// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "renderer/resources/terrain/layer_info.h"


namespace openage::renderer::resources {
class BlendTableInfo;
class Texture2dInfo;


/**
 * Contains information about a 2D terrain texture.
 */
class TerrainInfo {
public:
	/**
	 * Create a Terrain Info.
	 *
	 * @param scalefactor Factor by which sprite images are scaled down at default zoom level.
	 *                    Applies to all layers. Choosing a factor that is power of 2 is
	 *                    recommended.
	 * @param textures Information object of textures used by the terrain.
	 * @param layers Layer information.
	 * @param blendtable Blending table for setting edge blending patterns.
	 */
	TerrainInfo(const float scalefactor,
	            std::vector<std::shared_ptr<Texture2dInfo>> &textures,
	            std::vector<TerrainLayerInfo> &layers,
	            const std::shared_ptr<BlendTableInfo> &blendtable = nullptr);

	TerrainInfo() = default;
	~TerrainInfo() = default;

	/**
	 * Get the scaling factor of the terrain.
	 *
	 * @return A float containing the scaling factor.
	 */
	float get_scalefactor() const;

	/**
	 * Get number of textures referenced by the terrain.
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
	 * Get the blend table information of the layer.
	 *
	 * @return Blend table information object or \p nullptr if no blendtable is used.
	 */
	const std::shared_ptr<BlendTableInfo> &get_blendtable() const;

	/**
	 * Get number of layers in the terrain.
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
	const TerrainLayerInfo &get_layer(size_t idx) const;

private:
	/**
	 * Scaling factor of the terrain across all layers at default zoom level.
	 */
	float scalefactor;

	/**
	 * Information about textures used by the terrain.
	 */
	std::vector<std::shared_ptr<Texture2dInfo>> textures;

	/**
	 * Layer information.
	 */
	std::vector<TerrainLayerInfo> layers;

	/**
	 * Blending patterns at terrain edges.
	 */
	std::shared_ptr<BlendTableInfo> blendtable;
};

} // namespace openage::renderer::resources
