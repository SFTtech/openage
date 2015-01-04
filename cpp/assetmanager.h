// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_ASSETMANAGER_H_
#define OPENAGE_ASSETMANAGER_H_

#include "config.h"

#include <unordered_map>
#include <string>
#include <memory>

#include "texture.h"

namespace openage {

/**
 * Container class for all available assets.
 * Responsible for loading, providing and updating requested files.
 */
class AssetManager {
public:
	AssetManager(util::Dir *root);

	/**
	 * Test whether a requested asset filename can be loaded.
	 *
	 * @param name: asset filename.
	 * @returns this filename can be loaded.
	 */
	bool can_load(const std::string &name) const;

	/**
	 * Query the Texture for a given filename.
	 *
	 * @param name: the asset file name relative to the asset root.
	 * @returns the queried texture handle.
	 */
	Texture *get_texture(const std::string &name);

	/**
	 * Ask the kernel whether there were updates to watched files.
	 */
	void check_updates();

protected:
	typedef std::unique_ptr<Texture, void(*)(Texture*)> tex_ptr;
	
	/**
	 * Create an internal texture handle.
	 */
	Texture* load_texture(const std::string &name);
	
	/**
	 * Retrieves the texture for missing textures.
	 */
	tex_ptr get_missing_tex();

private:
	/**
	 * The root directory for the available assets.
	 */
	util::Dir *root;

	/**
	 * The replacement texture for missing textures.
	 */
	tex_ptr missing_tex;

	/**
	 * Map from texture filename to texture instance ptr.
	 */
	std::unordered_map<std::string, tex_ptr> textures;

#if WITH_INOTIFY
	/**
	 * The file descriptor pointing to the inotify instance.
	 */
	int inotify_fd;

	/**
	 * Map from inotify watch handle fd to texture instance ptr.
	 * The kernel returns the handle fd when events are triggered.
	 */
	std::unordered_map<int, Texture*> watch_fds;
#endif
};

}

#endif
