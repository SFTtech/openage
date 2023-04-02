// Copyright 2014-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "config.h"

#include <memory>
#include <string>
#include <unordered_map>

#include "util/path.h"

namespace qtsdl {
class GuiItemLink;
} // namespace qtsdl

namespace openage {
class Texture;
} // namespace openage

namespace openage::presenter {

/**
 * Container class for all available assets.
 * Responsible for loading, providing and updating requested files.
 */
class AssetManager final {
public:
	AssetManager();
	AssetManager(const util::Path &asset_dir);

	/**
	 * Return the path where assets are found in.
	 */
	const util::Path &get_asset_dir();

	/**
	 * Set the asset search path.
	 */
	void set_asset_dir(const util::Path &new_path);

	/**
	 * Query the Texture for a given filename.
	 *
	 * @param name: the asset file name relative to the asset root.
	 * @param use_metafile: load subtexture information from meta file
	 * @param null_if_missing: instead of providing the "missing texture",
	 *                         return nullptr.
	 * @returns the queried texture handle.
	 */
	Texture *get_texture(const std::string &name,
	                     bool use_metafile = true,
	                     bool null_if_missing = false);

	/**
	 * Ask the kernel whether there were updates to watched files.
	 */
	void check_updates();

protected:
	/**
	 * Create an internal texture handle.
	 */
	std::shared_ptr<Texture> load_texture(const std::string &name,
	                                      bool use_metafile = true,
	                                      bool null_if_missing = false);

	/**
	 * Retrieves the texture for missing textures.
	 */
	std::shared_ptr<Texture> get_missing_tex();

private:
	void clear();

	/**
	 * The root directory for the available assets.
	 */
	util::Path asset_dir;

	/**
	 * The replacement texture for missing textures.
	 */
	std::shared_ptr<Texture> missing_tex;

	/**
	 * Map from texture filename to texture instance ptr.
	 */
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

#if WITH_INOTIFY
	/**
	 * The file descriptor pointing to the inotify instance.
	 */
	int inotify_fd;

	/**
	 * Map from inotify watch handle fd to texture instance ptr.
	 * The kernel returns the handle fd when events are triggered.
	 */
	std::unordered_map<int, std::shared_ptr<Texture>> watch_fds;
#endif
};

} // namespace openage::presenter
