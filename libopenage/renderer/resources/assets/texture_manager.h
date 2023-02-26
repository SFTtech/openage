// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "util/path.h"

namespace openage::renderer {
class Renderer;
class Texture2d;

namespace resources {

using texture_cache_t = std::unordered_map<std::string, std::shared_ptr<Texture2d>>;

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
     * Get the corresponding texture for the specified path.
     *
     * If the texture does not exist in the cache yet, it will be loaded
     * using the given path.
     *
     * @param path Path to the texture resource.
     * @param info_file If true, interpret the path as a texture info file, else as
     *                  an image data file.
     *
     * @return Texture resource at the given path.
     */
	const std::shared_ptr<Texture2d> &request(const util::Path &path, bool info_file = false);

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
     * Get the cache with the loaded textures.
     *
     * @param path Map of filenames and loaded textures.
     */
	const texture_cache_t &get_cache();

private:
	/**
     * openage renderer.
     */
	std::shared_ptr<Renderer> renderer;

	/**
     * Cache of already created textures.
     */
	texture_cache_t loaded;
};

} // namespace resources
} // namespace openage::renderer
