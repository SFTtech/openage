// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "util/path.h"


namespace openage::renderer {
class Renderer;
class Texture2d;

namespace resources {

/**
 * Loads and stores references to shared texture assets.
 *
 * Using the texture manager to request texture assets allows quick access
 * to already loaded assets and avoid creating unnecessary duplicates.
 */
class TextureManager {
public:
	/**
	 * Create a new texture manager.
	 *
	 * @param renderer The openage renderer instance.
	 */
	TextureManager(const std::shared_ptr<Renderer> &renderer);
	~TextureManager() = default;

	/**
	 * Prevent accidental copy or assignment because it would defeat the
	 * point of a persistent cache.
	 */
	TextureManager(const TextureManager &) = delete;
	TextureManager &operator=(const TextureManager &) = delete;

	/**
	 * Get the corresponding texture for the specified path.
	 *
	 * If the texture does not exist in the cache yet, it will be loaded
	 * using the given path.
	 *
	 * @param path Path to the texture resource.
	 *
	 * @return Texture resource at the given path.
	 */
	const std::shared_ptr<Texture2d> &request(const util::Path &path);

	/**
	 * Load the texture at the given path. Does nothing if the path
	 * already exists in the cache.
	 *
	 * @param path Path to the texture resource.
	 */
	void add(const util::Path &path);

	/**
	 * Assign a specific texture to the given path. Overwrites existing
	 * textures references if the path already exists in the cache.
	 *
	 * @param path Path to the texture resource.
	 * @param texture Existing texture object.
	 */
	void add(const util::Path &path,
	         const std::shared_ptr<Texture2d> &texture);

	/**
	 * Remove a texture reference from the cache.
	 *
	 * @param path Path to the texture resource.
	 */
	void remove(const util::Path &path);

	/**
	 * Set the placeholder texture.
	 *
	 * @param path Path to the texture resource.
	 */
	void set_placeholder(const util::Path &path);

	using placeholder_t = std::optional<std::pair<util::Path, std::shared_ptr<Texture2d>>>;

	/**
	 * Get the placeholder texture.
	 *
	 * @return Placeholder texture. Can be \p nullptr.
	 */
	const placeholder_t &get_placeholder() const;

private:
	/**
	 * openage renderer.
	 */
	std::shared_ptr<Renderer> renderer;

	using texture_cache_t = std::unordered_map<util::Path, std::shared_ptr<Texture2d>>;

	/**
	 * Cache of already created textures.
	 */
	texture_cache_t loaded;

	/**
	 * Placeholder texture to use if a texture could not be loaded.
	 */
	placeholder_t placeholder;
};

} // namespace resources
} // namespace openage::renderer
