// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "renderer/resources/assets/texture_manager.h"
#include "util/path.h"

namespace openage::renderer {
class Renderer;

namespace resources {
class Animation2dInfo;
class BlendPatternInfo;
class BlendTableInfo;
class PaletteInfo;
class TerrainInfo;

/**
 * Loads and stores references to shared graphics assets such as textures,
 * animations, palettes, etc.
 *
 * Using the asset manager allows quick access to already loaded assets and avoids
 * creating unnecessary duplicates.
 */
class AssetManager {
public:
	/**
     * Create a new asset manager.
     *
     * @param renderer The openage renderer instance.
     */
	AssetManager(const std::shared_ptr<Renderer> &renderer);
	~AssetManager() = default;

	/**
     * Get the corresponding asset for the specified path.
     *
     * If the asset does not exist in the cache yet, it will be loaded
     * using the given path.
     *
     * @param path Path to the asset resource.
     *
     * @return Texture resource at the given path.
     */
	const std::shared_ptr<Animation2dInfo> &request_animation(const util::Path &path);
	const std::shared_ptr<BlendPatternInfo> &request_blpattern(const util::Path &path);
	const std::shared_ptr<BlendTableInfo> &request_bltable(const util::Path &path);
	const std::shared_ptr<PaletteInfo> &request_palette(const util::Path &path);
	const std::shared_ptr<TerrainInfo> &request_terrain(const util::Path &path);

	/**
     * Remove an asset reference from the cache.
     *
     * @param path Path to the asset resource.
     */
	void remove_animation(const util::Path &path);
	void remove_blpattern(const util::Path &path);
	void remove_bltable(const util::Path &path);
	void remove_palette(const util::Path &path);
	void remove_terrain(const util::Path &path);

private:
	/**
     * Manages individual textures/image resources used by the
     * high level asset formats cached by the asset manager.
     */
	TextureManager texture_manager;

	using anim_cache_t = std::unordered_map<std::string, std::shared_ptr<Animation2dInfo>>;
	using blpattern_cache_t = std::unordered_map<std::string, std::shared_ptr<BlendPatternInfo>>;
	using bltable_cache_t = std::unordered_map<std::string, std::shared_ptr<BlendTableInfo>>;
	using palette_cache_t = std::unordered_map<std::string, std::shared_ptr<PaletteInfo>>;
	using terrain_cache_t = std::unordered_map<std::string, std::shared_ptr<TerrainInfo>>;

	/**
     * Cache of already loaded animations.
     */
	anim_cache_t loaded_animations;

	/**
     * Cache of already loaded blending patterns.
     */
	blpattern_cache_t loaded_blpatterns;

	/**
     * Cache of already loaded blending tables.
     */
	bltable_cache_t loaded_bltables;

	/**
     * Cache of already loaded colour palettes.
     */
	palette_cache_t loaded_palettes;

	/**
     * Cache of already loaded terrains.
     */
	terrain_cache_t loaded_terrains;
};

} // namespace resources
} // namespace openage::renderer
