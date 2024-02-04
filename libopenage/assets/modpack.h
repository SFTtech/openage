// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "util/path.h"


namespace openage::assets {

/**
 * Author information.
 */
struct AuthorInfo {
	std::string name;
	std::string fullname;
	std::string since;
	std::string until;
	std::vector<std::string> roles;
	std::unordered_map<std::string, std::string> contact;
};

/**
 * Author group information.
 */
struct AuthorGroupInfo {
	std::string name;
	std::string description;
	std::vector<std::string> authors;
};

/**
 * Modpack metadata information.
 */
struct ModpackInfo {
	// modpack location (directory)
	util::Path path;

	// modpack information
	// see /doc/media/openage/modpack_definition_file.md
	std::string id;
	std::string version;
	std::string versionstr;
	std::string repo;
	std::string alias;
	std::string title;
	std::string description;
	std::string long_description;
	std::string url;
	std::vector<std::string> licenses;

	std::vector<std::string> dependencies;
	std::vector<std::string> conflicts;

	std::vector<std::string> includes;
	std::vector<std::string> excludes;

	std::vector<AuthorInfo> authors;
	std::vector<AuthorGroupInfo> author_groups;
};

/**
 * Parse a modpack definition file.
 *
 * @param info_file openage modpack definition file.
 * @return Modpack metadata information.
 */
ModpackInfo parse_modepack_def(const util::Path &info_file);

/**
 * Modpack definition.
 *
 * Modpacks are a collection of assets (data files, graphics, sounds, etc.)
 * that can be loaded into the engine.
 */
class Modpack {
public:
	/**
	 * Create a new modpack.
	 *
	 * Loads the modpack using the information in the definition file.
	 *
	 * @param info_file Path to the modpack definition file.
	 */
	Modpack(const util::Path &info_file);

	/**
	 * Create a new modpack from an existing modpack info.
	 *
	 * @param info Modpack metadata information.
	 */
	Modpack(const ModpackInfo &info);

	/**
	 * Create a new modpack from an existing modpack info.
	 *
	 * @param info Modpack metadata information.
	 */
	Modpack(const ModpackInfo &&info);

	Modpack(const Modpack &) = delete;
	Modpack(Modpack &&) = default;
	~Modpack() = default;

	/**
	 * Get the metadata information of the modpack.
	 *
	 * @return Modpack metadata information.
	 */
	const ModpackInfo &get_info() const;

	/**
	 * Check if the modpack is valid.
	 *
	 * @return true if the modpack is valid, false otherwise.
	 */
	bool check_integrity() const;

private:
	/**
	 * Modpack metadata information.
	 */
	ModpackInfo info;
};

} // namespace openage::assets
