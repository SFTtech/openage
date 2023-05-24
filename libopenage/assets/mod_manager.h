// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

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
	void register_modpacks(const ModpackInfo &info);

	/**
     * Load all modpacks with the given IDs. Modpacks must be registered
     * with \p register_modpack() before loading.
     *
     * Note that this technically only loads modpack metadata. The actual gamedata
     * is loaded by the gamestate when a game is created or, in case of media files,
     * on-demand by the asset manager.
     *
     * @param modpack_id ID of the modpack to load.
     */
	void load_modpacks(const std::vector<std::string> &load_order);

	/**
     * Set the order in which modpack data should be loaded.
     *
     * This also checks whether the given load order is valid, i.e.
     * by checking if all dependencies/conflicts are resolved.
     *
     * @param load_order Load order of modpacks.
     */
	void set_load_order(const std::vector<std::string> &load_order);

	/**
     * Get the load order of modpacks.
     *
     * @return Load order of modpacks.
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
	static std::vector<ModpackInfo> enumerate_modpacks(const util::Path &directory);

private:
	/**
     * Loaded modpacks.
     */
	std::unordered_map<std::string, std::shared_ptr<Modpack>> loaded;

	/**
     * Load order of modpacks.
     */
	std::vector<std::string> load_order;

	/**
     * Available modpacks (mapping modpack ID to modpack info struct).
     */
	std::unordered_map<std::string, ModpackInfo> available;
};

} // namespace openage::assets
