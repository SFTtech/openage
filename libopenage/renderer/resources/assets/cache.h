// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "util/path.h"


namespace openage::renderer::resources {
class Animation2dInfo;
class BlendPatternInfo;
class BlendTableInfo;
class PaletteInfo;
class TerrainInfo;
class Texture2dInfo;

/**
 * Loads and stores references to shared asset descriptions such as texture info,
 * animations, palettes, etc.
 *
 * Using the asset manager allows quick access to already loaded assets and avoids
 * creating unnecessary duplicates.
 */
class AssetCache {
public:
	/**
	 * Create a new asset cache.
	 */
	AssetCache() = default;
	~AssetCache() = default;

	/**
	 * Get the corresponding asset for the specified path.
	 *
	 * @param path Path to the asset resource.
	 *
	 * @return Asset resource at the given path.
	 * @throw Error if the resource is not in the cache.
	 */
	const std::shared_ptr<Animation2dInfo> &get_animation(const util::Path &path);
	const std::shared_ptr<BlendPatternInfo> &get_blpattern(const util::Path &path);
	const std::shared_ptr<BlendTableInfo> &get_bltable(const util::Path &path);
	const std::shared_ptr<PaletteInfo> &get_palette(const util::Path &path);
	const std::shared_ptr<TerrainInfo> &get_terrain(const util::Path &path);
	const std::shared_ptr<Texture2dInfo> &get_texture(const util::Path &path);
	// TODO: use std::optional<std::shared_ptr<Texture2dInfo>>

	/**
	 * Map a specific asset info to the given path and add it to the cache.
	 * Overwrites existing references if the path already exists in the cache.
	 *
	 * @param path Path to the asset resource.
	 * @param info Existing asset object.
	 */
	void add_animation(const util::Path &path, const std::shared_ptr<Animation2dInfo> info);
	void add_blpattern(const util::Path &path, const std::shared_ptr<BlendPatternInfo> info);
	void add_bltable(const util::Path &path, const std::shared_ptr<BlendTableInfo> info);
	void add_palette(const util::Path &path, const std::shared_ptr<PaletteInfo> info);
	void add_terrain(const util::Path &path, const std::shared_ptr<TerrainInfo> info);
	void add_texture(const util::Path &path, const std::shared_ptr<Texture2dInfo> info);

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
	void remove_texture(const util::Path &path);

	/**
	 * Check if an asset reference is in the cache.
	 *
	 * @param path Path to the asset resource.
	 *
	 * @return true if the resource is cached, else false.
	 */
	bool check_animation_cache(const util::Path &path);
	bool check_blpattern_cache(const util::Path &path);
	bool check_bltable_cache(const util::Path &path);
	bool check_palette_cache(const util::Path &path);
	bool check_terrain_cache(const util::Path &path);
	bool check_texture_cache(const util::Path &path);

private:
	using anim_cache_t = std::unordered_map<util::Path, std::shared_ptr<Animation2dInfo>>;
	using blpattern_cache_t = std::unordered_map<util::Path, std::shared_ptr<BlendPatternInfo>>;
	using bltable_cache_t = std::unordered_map<util::Path, std::shared_ptr<BlendTableInfo>>;
	using palette_cache_t = std::unordered_map<util::Path, std::shared_ptr<PaletteInfo>>;
	using terrain_cache_t = std::unordered_map<util::Path, std::shared_ptr<TerrainInfo>>;
	using texture_cache_t = std::unordered_map<util::Path, std::shared_ptr<Texture2dInfo>>;

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

	/**
	 * Cache of already loaded textures.
	 */
	texture_cache_t loaded_textures;
};

} // namespace openage::renderer::resources
