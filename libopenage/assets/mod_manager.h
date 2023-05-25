// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "log/log.h"

#include "assets/modpack.h"
#include "util/path.h"

namespace openage::assets {

class ModManager {
public:
	ModManager() = default;
	~ModManager() = default;

	/**
     * Adds a modpack to the list of available modpacks.
     *
     * This does not load the modpack yet as we still need to
     * determine the final load order and resolve dependecies.
     *
     * @param info_file Path to the modpack definition file.
     */
	void register_modpack(const util::Path &info_file);

	/**
     * Adds a modpack to the list of available modpacks.
     *
     * This does not load the modpack yet as we still need to
     * determine the final load order and resolve dependecies.
     *
     * @param info Modpack definition.
     */
	void register_modpack(const ModpackInfo &info);

	/**
     * Load all modpacks with the given IDs. Modpacks must be registered
     * with \p register_modpack() before loading.
     *
     * TODO: Mount modpacks into virtual filesystem.
     *
     * Note that this technically only loads modpack metadata. The actual gamedata
     * is loaded by the gamestate when a game is created or, in case of media files,
     * on-demand by the asset manager.
     *
     * @param modpack_id ID of the modpack to load.
     */
	void load_modpacks(const std::vector<std::string> &load_order);

	/**
     * Get a loaded modpack by its ID.
     *
     * @param modpack_id ID of the modpack to get.
     *
     * @return Modpack with the given ID.
     */
	std::shared_ptr<Modpack> get_modpack(const std::string &modpack_id) const;

	/**
     * Get the load order of modpacks.
     *
     * @return Modpack IDs in the order that they should be loaded.
     */
	const std::vector<std::string> &get_load_order() const;

	/**
     * Enumerates all modpack ids in a given directory.
     *
     * This also loads available modpack definition files.
     *
     * @param directory Path to the directory to enumerate.
     *
     * @return Infos of the identified modpacks.
     */
	static std::vector<ModpackInfo> enumerate_modpacks(const util::Path &directory) {
		std::vector<ModpackInfo> result;

		if (not(directory.exists() and directory.is_dir())) {
			throw Error{MSG(err) << "Modpack directory '" << directory << "' does not exist."};
		}

		auto dir = const_cast<util::Path &>(directory);
		for (auto entry : dir.iterdir()) {
			if (entry.is_dir()) {
				auto info_file = entry / "modpack.toml";
				if (info_file.exists()) {
					result.push_back(parse_modepack_def(info_file));
					log::log(INFO << "Found modpack: " << result.back().id);
				}
			}
		}

		return result;
	}

private:
	/**
     * Set the order in which modpack data should be loaded.
     *
     * This also checks whether the given load order is valid, i.e.
     * by checking if all dependencies/conflicts are resolved.
     *
     * TODO: Dynamically resolve load order?
     *
     * @param load_order Load order of modpacks.
     */
	void set_load_order(const std::vector<std::string> &load_order);

	/**
     * Loaded modpacks.
     */
	std::unordered_map<std::string, std::shared_ptr<Modpack>> loaded;

	/**
     * Available modpacks (mapping modpack ID to modpack info struct).
     */
	std::unordered_map<std::string, ModpackInfo> available;

	/**
     * Load order of modpacks.
     */
	std::vector<std::string> load_order;
};

} // namespace openage::assets
