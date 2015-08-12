// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "util/dir.h"
#include "watch/watch.h"

namespace qtsdl {
class GuiItemLink;
} // namespace qtsdl

namespace openage {

class Texture;

/**
 * Container class for all available assets.
 * Responsible for loading, providing and updating requested files.
 */
class AssetManager final {
public:
	explicit AssetManager(qtsdl::GuiItemLink *gui_link);

	util::Dir *get_data_dir();

	std::string get_data_dir_string() const;
	void set_data_dir_string(const std::string& data_dir);

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
	Texture *get_texture(const std::string &name, bool use_metafile=true);

	/**
	 * Ask the kernel whether there were updates to watched files.
	 */
	void check_updates();

protected:
	/**
	 * File change monitoring and automatic reloading.
	 */
	std::unique_ptr<watch::WatchManager> watch_manager;

	/**
	 * Create an internal texture handle.
	 */
	std::shared_ptr<Texture> load_texture(const std::string &name, bool use_metafile=true);

	/**
	 * Retrieves the texture for missing textures.
	 */
	std::shared_ptr<Texture> get_missing_tex();

private:
	void clear();

	/**
	 * The root directory for the available assets.
	 */
	util::Dir root;

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
