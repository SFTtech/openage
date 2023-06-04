// Copyright 2014-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "config.h"

#include <memory>
#include <string>
#include <unordered_map>

#include "presenter/legacy/legacy.h"
#include "util/path.h"

namespace qtsdl {
class GuiItemLink;
} // namespace qtsdl

namespace openage {
class Texture;

namespace gamestate {
class GameSimulation;
}

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
	void set_asset_dir(const util::Path &asset_dir);

	/**
	 * Set the game display of this asset manager.
	 * Called from QML.
	 */
	void set_display(presenter::LegacyDisplay *display);

	/**
	 * Return the display responsible for this asset manager.
	 */
	presenter::LegacyDisplay *get_display() const;

	/**
	 * Set the game engine of this asset manager.
	 * Called from QML.
	 */
	void set_engine(gamestate::GameSimulation *engine);

	/**
	 * Return the engine responsible for this asset manager.
	 */
	gamestate::GameSimulation *get_engine() const;

	/**
	 * Query the Texture for a given filename.
	 *
	 * @param name: the asset file name relative to the asset root.
	 * @param use_metafile: load subtexture information from meta file
	 * @param null_if_missing: instead of providing the "missing texture",
	 *                         return nullptr.
	 * @returns the queried texture handle.
	 */
	Texture *get_texture(const std::string &name, bool use_metafile = true, bool null_if_missing = false);

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
	 * The display this asset manager is attached to.
	 */
	presenter::LegacyDisplay *display;

	/**
	 * The engine this asset manager is attached to.
	 */
	gamestate::GameSimulation *engine;

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

} // namespace openage
