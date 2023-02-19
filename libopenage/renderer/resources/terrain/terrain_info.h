// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "renderer/resources/terrain/layer_info.h"
#include "renderer/resources/texture_info.h"

namespace openage::renderer::resources {

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
	 */
	TerrainInfo(const std::shared_ptr<BlendTableInfo> &blendtable,
	            const float scalefactor,
	            std::vector<Texture2dInfo> &textures,
	            std::vector<TerrainLayerInfo> &layers);

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
	const Texture2dInfo &get_texture(size_t idx) const;

	/**
	 * Get the blend table information of the layer.
	 *
	 * @return Blend table information object.
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
	 * Blending patterns at terrain edges.
	 */
	std::shared_ptr<BlendTableInfo> blendtable;

	/**
	 * Information about textures used by the terrain.
	 */
	std::vector<Texture2dInfo> textures;

	/**
	 * Layer information.
	 */
	std::vector<TerrainLayerInfo> layers;
};

} // namespace openage::renderer::resources
