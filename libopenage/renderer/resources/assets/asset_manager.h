// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "util/path.h"


namespace openage::renderer {
class Renderer;

namespace resources {
class AssetCache;
class TextureManager;

class Animation2dInfo;
class BlendPatternInfo;
class BlendTableInfo;
class PaletteInfo;
class TerrainInfo;
class Texture2dInfo;

/**
 * Loads and stores references to shared graphics assets such as textures,
 * animations, palettes, etc.
 *
 * Using the asset manager allows quick access to already loaded assets and avoids
 * creating unnecessary duplicates.
 *
 * TODO: Hot reloading/cache invalidation with inotify.
 */
class AssetManager {
public:
	/**
	 * Create a new asset manager.
	 *
	 * @param renderer The openage renderer instance.
	 * @param asset_base_dir Base path for all assets.
	 */
	AssetManager(const std::shared_ptr<Renderer> &renderer,
	             const util::Path &asset_base_dir);
	~AssetManager() = default;

	/**
	 * Prevent accidental copy or assignment because it would defeat the
	 * point of a persistent cache.
	 */
	AssetManager(const AssetManager &) = delete;
	AssetManager &operator=(const AssetManager &) = delete;

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
	const std::shared_ptr<Texture2dInfo> &request_texture(const util::Path &path);

	/**
	 * Get the corresponding asset for a path string relative to the
	 * asset base directory.
	 *
	 * If the asset does not exist in the cache yet, it will be loaded
	 * using the given path.
	 *
	 * @param path Relative path to the asset resource (from the asset base dir).
	 *
	 * @return Texture resource at the given path.
	 */
	const std::shared_ptr<Animation2dInfo> &request_animation(const std::string &rel_path);
	const std::shared_ptr<BlendPatternInfo> &request_blpattern(const std::string &rel_path);
	const std::shared_ptr<BlendTableInfo> &request_bltable(const std::string &rel_path);
	const std::shared_ptr<PaletteInfo> &request_palette(const std::string &rel_path);
	const std::shared_ptr<TerrainInfo> &request_terrain(const std::string &rel_path);
	const std::shared_ptr<Texture2dInfo> &request_texture(const std::string &rel_path);

	using placeholder_anim_t = std::optional<std::pair<util::Path, std::shared_ptr<Animation2dInfo>>>;
	using placeholder_blpattern_t = std::optional<std::pair<util::Path, std::shared_ptr<BlendPatternInfo>>>;
	using placeholder_bltable_t = std::optional<std::pair<util::Path, std::shared_ptr<BlendTableInfo>>>;
	using placeholder_palette_t = std::optional<std::pair<util::Path, std::shared_ptr<PaletteInfo>>>;
	using placeholder_terrain_t = std::optional<std::pair<util::Path, std::shared_ptr<TerrainInfo>>>;
	using placeholder_texture_t = std::optional<std::pair<util::Path, std::shared_ptr<Texture2dInfo>>>;

	/**
	 * Set a placeholder asset that is returned in case a regular request cannot
	 * find the requested asset.
	 *
	 * @param path Path to the placeholder asset resource.
	 */
	void set_placeholder_animation(const util::Path &path);
	void set_placeholder_blpattern(const util::Path &path);
	void set_placeholder_bltable(const util::Path &path);
	void set_placeholder_palette(const util::Path &path);
	void set_placeholder_terrain(const util::Path &path);
	void set_placeholder_texture(const util::Path &path);

	/**
	 * Get the placeholder asset for a specific asset type.
	 *
	 * @return Placeholder asset resource.
	 */
	const placeholder_anim_t &get_placeholder_animation();
	const placeholder_blpattern_t &get_placeholder_blpattern();
	const placeholder_bltable_t &get_placeholder_bltable();
	const placeholder_palette_t &get_placeholder_palette();
	const placeholder_terrain_t &get_placeholder_terrain();
	const placeholder_texture_t &get_placeholder_texture();

	/**
	 * Get the texture manager for accessing cached image resources.
	 *
	 * @return Texture manager.
	 */
	const std::shared_ptr<TextureManager> &get_texture_manager();

private:
	/**
	 * openage renderer.
	 */
	std::shared_ptr<Renderer> renderer;

	/**
	 * Cache of already loaded assets.
	 */
	std::shared_ptr<AssetCache> cache;

	/**
	 * Manages individual textures/image resources used by the
	 * high level asset formats cached by the asset manager.
	 */
	std::shared_ptr<TextureManager> texture_manager;

	/**
	 * Base path for all assets.
	 *
	 * TODO: This should be the mount point of modpacks?
	 */
	util::Path asset_base_dir;

	/**
	 * Placeholder assets that can be used if a resource is not found.
	 */
	placeholder_anim_t placeholder_animation;
	placeholder_blpattern_t placeholder_blpattern;
	placeholder_bltable_t placeholder_bltable;
	placeholder_palette_t placeholder_palette;
	placeholder_terrain_t placeholder_terrain;
	placeholder_texture_t placeholder_texture;
};

} // namespace resources
} // namespace openage::renderer
