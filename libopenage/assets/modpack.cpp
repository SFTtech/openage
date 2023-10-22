// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "modpack.h"

#include <toml.hpp>

#include "error/error.h"

namespace openage::assets {

ModpackInfo parse_modepack_def(const util::Path &info_file) {
	ModpackInfo def;
	def.path = info_file.get_parent();

	const auto modpack_def = toml::parse(info_file.resolve_native_path());

	// info table
	const toml::table info = toml::find<toml::table>(modpack_def, "info");
	if (info.contains("name")) {
		def.id = info.at("name").as_string();
	}
	else {
		throw Error{MSG(err) << "Modpack definition file at " << info_file
		                     << " 'info' table misses 'name' parameter."};
	}

	if (info.contains("version")) {
		def.version = info.at("version").as_string();
	}
	else {
		throw Error{MSG(err) << "Modpack definition file at " << info_file
		                     << " 'info' table misses 'version' parameter."};
	}

	// optionals
	if (info.contains("versionstr")) {
		def.versionstr = info.at("versionstr").as_string();
	}
	if (info.contains("repo")) {
		def.repo = info.at("repo").as_string();
	}
	if (info.contains("alias")) {
		def.alias = info.at("alias").as_string();
	}
	if (info.contains("title")) {
		def.title = info.at("title").as_string();
	}
	if (info.contains("description")) {
		def.description = info.at("description").as_string();
	}
	if (info.contains("long_description")) {
		def.long_description = info.at("long_description").as_string();
	}
	if (info.contains("url")) {
		def.url = info.at("url").as_string();
	}
	if (info.contains("licenses")) {
		std::vector<std::string> licenses{};
		for (const auto &license : info.at("licenses").as_array()) {
			licenses.push_back(license.as_string());
		}
		def.licenses = licenses;
	}

	// assets table
	const toml::table assets = toml::find<toml::table>(modpack_def, "assets");
	std::vector<std::string> includes{};
	for (const auto &include : assets.at("include").as_array()) {
		includes.push_back(include.as_string());
	}
	def.includes = includes;

	// optionals
	if (assets.contains("exclude")) {
		std::vector<std::string> excludes{};
		for (const auto &exclude : assets.at("exclude").as_array()) {
			excludes.push_back(exclude.as_string());
		}
		def.excludes = excludes;
	}

	// dependency table
	if (modpack_def.contains("dependency")) {
		const toml::table dependency = toml::find<toml::table>(modpack_def, "dependency");
		std::vector<std::string> deps{};

		if (not dependency.contains("modpacks")) {
			throw Error{MSG(err) << "Modpack definition file at " << info_file
			                     << " 'dependency' table misses 'modpacks' parameter."};
		}

		for (const auto &dep : dependency.at("modpacks").as_array()) {
			deps.push_back(dep.as_string());
		}
		def.dependencies = deps;
	}

	// conflicts table
	if (modpack_def.contains("conflict")) {
		const toml::table conflict = toml::find<toml::table>(modpack_def, "conflict");
		std::vector<std::string> conflicts{};

		if (not conflict.contains("modpacks")) {
			throw Error{MSG(err) << "Modpack definition file at " << info_file
			                     << " 'conflict' table misses 'modpacks' parameter."};
		}

		for (const auto &cf : conflict.at("modpacks").as_array()) {
			conflicts.push_back(cf.as_string());
		}
		def.conflicts = conflicts;
	}

	// authors table
	if (modpack_def.contains("authors")) {
		const toml::table authors = toml::find<toml::table>(modpack_def, "authors");
		std::vector<AuthorInfo> author_infos{};
		for (const auto &author : authors) {
			AuthorInfo author_info{};
			if (author.second.contains("name")) {
				author_info.name = author.second.at("name").as_string();
			}
			else {
				throw Error{MSG(err) << "Modpack definition file at " << info_file
				                     << " 'author' table misses 'name' parameter."};
			}

			// optionals
			if (author.second.contains("fullname")) {
				author_info.fullname = author.second.at("fullname").as_string();
			}
			if (author.second.contains("since")) {
				author_info.since = author.second.at("since").as_string();
			}
			if (author.second.contains("until")) {
				author_info.until = author.second.at("until").as_string();
			}
			if (author.second.contains("roles")) {
				std::vector<std::string> roles{};
				for (const auto &role : author.second.at("roles").as_array()) {
					roles.push_back(role.as_string());
				}
				author_info.roles = roles;
			}
			if (author.second.contains("contact")) {
				const toml::table contact = toml::find<toml::table>(author.second, "contact");
				std::unordered_map<std::string, std::string> contacts{};
				for (const auto &contact_info : contact) {
					contacts[contact_info.first] = contact_info.second.as_string();
				}
				author_info.contact = contacts;
			}

			author_infos.push_back(author_info);
		}
		def.authors = author_infos;
	}

	// authorgroups table
	if (modpack_def.contains("authorgroups")) {
		const toml::table authorgroups = toml::find<toml::table>(modpack_def, "authorgroups");
		std::vector<AuthorGroupInfo> author_group_infos{};
		for (const auto &authorgroup : authorgroups) {
			AuthorGroupInfo author_group_info{};
			if (authorgroup.second.contains("name")) {
				author_group_info.name = authorgroup.second.at("name").as_string();
			}
			else {
				throw Error{MSG(err) << "Modpack definition file at " << info_file
				                     << " 'authorgroups' table misses 'name' parameter."};
			}

			if (authorgroup.second.contains("authors")) {
				std::vector<std::string> authors{};
				for (const auto &author : authorgroup.second.at("authors").as_array()) {
					authors.push_back(author.as_string());
				}
				author_group_info.authors = authors;
			}
			else {
				throw Error{MSG(err) << "Modpack definition file at " << info_file
				                     << " 'authorgroups' table misses 'authors' parameter."};
			}

			// optionals
			if (authorgroup.second.contains("description")) {
				author_group_info.description = authorgroup.second.at("description").as_string();
			}

			author_group_infos.push_back(author_group_info);
		}
		def.author_groups = author_group_infos;
	}

	return def;
}


Modpack::Modpack(const util::Path &info_file) :
	info{parse_modepack_def(info_file)} {
}

Modpack::Modpack(const ModpackInfo &info) :
	info{info} {}

Modpack::Modpack(const ModpackInfo &&info) :
	info{info} {
}

const ModpackInfo &Modpack::get_info() const {
	return this->info;
}

bool Modpack::check_integrity() const {
	// TODO: Check manifest file
	throw Error{MSG(err) << "Integrity check not implemented yet."};
}

} // namespace openage::assets
