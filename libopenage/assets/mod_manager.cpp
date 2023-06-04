// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "mod_manager.h"

#include <unordered_set>

namespace openage::assets {

void ModManager::register_modpack(const util::Path &info_file) {
	auto info = parse_modepack_def(info_file);
	this->available.emplace(info.id, info);
}

void ModManager::register_modpacks(const ModpackInfo &info) {
	this->available.emplace(info.id, info);
}

void ModManager::load_modpacks(const std::vector<std::string> &load_order) {
	this->set_load_order(load_order);

	for (const auto &modpack_id : load_order) {
		auto &modpack = this->available.at(modpack_id);
		this->loaded.emplace(modpack_id, std::make_shared<Modpack>(modpack));
	}
}

void ModManager::set_load_order(const std::vector<std::string> &load_order) {
	std::unordered_set<std::string> requested{load_order.begin(), load_order.end()};

	// sanity check: availability, dependencies, conflicts
	for (const auto &modpack_id : load_order) {
		if (not this->available.contains(modpack_id)) {
			throw Error{MSG(err) << "Requested modpack '" << modpack_id << "' not available."};
		}

		auto &info = this->available.at(modpack_id);
		for (const auto &dependency : info.dependencies) {
			if (not requested.contains(dependency)) {
				throw Error{MSG(err) << "Dependency '" << dependency << "' of modpack '"
				                     << modpack_id << "' not satisfied"};
			}
		}

		for (const auto &conflict : info.conflicts) {
			if (requested.contains(conflict)) {
				throw Error{MSG(err) << "Modpack '" << modpack_id << "' conflicts with modpack '"
				                     << conflict << "'"};
			}
		}
	}

	this->load_order = load_order;
}


} // namespace openage::assets
