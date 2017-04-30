// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "config.h"

#include <unordered_map>
#include <string>
#include <memory>

#include "util/path.h"

class SDL_Surface;
namespace qtsdl {
class GuiItemLink;
} // namespace qtsdl

namespace openage {

class Engine;
class Texture;

/**
 * Container class for all available assets.
 * Responsible for loading, providing and updating requested files.
 */
class AssetManager final {
public:
	explicit AssetManager(qtsdl::GuiItemLink *gui_link);

	/**
	 * Return the path where assets are found in.
	 */
	const util::Path &get_asset_dir();

	/**
	 * Set the asset search path.
	 */
	void set_asset_dir(const util::Path& asset_dir);

	/**
	 * Set the game engine of this asset manager.
	 * Called from QML.
	 */
	void set_engine(Engine *engine);

	/**
	 * Return the engine responsible for this asset manager.
	 */
	Engine *get_engine() const;

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
	                     bool null_if_missing = false,
	                     SDL_Surface *surface = nullptr);

	/**
	 * Ask the kernel whether there were updates to watched files.
	 */
	void check_updates();

	void load_textures(const std::vector<std::string> &names,
	                   bool use_metafile = true,
	                   bool null_if_missing = false);

protected:
	/**
	 * Create an internal texture handle.
	 */
	std::shared_ptr<Texture> load_texture(const std::string &name,
	                                      bool use_metafile=true,
	                                      bool null_if_missing=false,
					      SDL_Surface *surface=nullptr);

	/**
	 * Retrieves the texture for missing textures.
	 */
	std::shared_ptr<Texture> get_missing_tex();

private:
	void clear();

	/**
	 * The engine this asset manager is attached to.
	 */
	Engine *engine;

	/**
	 * The root directory for the available assets.
	 */
	util::Path asset_path;

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

public:
	qtsdl::GuiItemLink *gui_link;
};

}
