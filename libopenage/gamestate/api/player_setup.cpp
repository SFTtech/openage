// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "player_setup.h"

#include <deque>
#include <memory>
#include <string>
#include <unordered_set>

#include <nyan/nyan.h>


namespace openage::gamestate::api {

bool APIPlayerSetup::is_player_setup(const nyan::Object &obj) {
	return obj.extends("engine.util.setup.PlayerSetup");
}

const std::vector<nyan::Object> APIPlayerSetup::get_modifiers(const nyan::Object &player_setup) {
	std::vector<nyan::Object> result;

	auto db_view = player_setup.get_view();
	auto modifiers = player_setup.get_set("PlayerSetup.modifiers");
	for (auto &modifier_val : modifiers) {
		auto modifier_obj_val = modifier_val.get_value_ptr<nyan::ObjectValue>();
		auto modifier_obj = db_view->get_object(modifier_obj_val->get_name());
		result.push_back(modifier_obj);
	}

	return result;
}

const std::vector<nyan::Object> APIPlayerSetup::get_start_resources(const nyan::Object &player_setup) {
	std::vector<nyan::Object> result;

	auto db_view = player_setup.get_view();
	auto start_resources = player_setup.get_set("PlayerSetup.starting_resources");
	for (auto &resource_val : start_resources) {
		auto resource_obj_val = resource_val.get_value_ptr<nyan::ObjectValue>();
		auto resource_obj = db_view->get_object(resource_obj_val->get_name());
		result.push_back(resource_obj);
	}

	return result;
}

const std::vector<nyan::Object> APIPlayerSetup::get_patches(const nyan::Object &player_setup) {
	std::vector<nyan::Object> result;

	auto db_view = player_setup.get_view();
	auto patches = player_setup.get_set("PlayerSetup.game_setup");
	for (auto &patch_val : patches) {
		auto patch_obj_val = patch_val.get_value_ptr<nyan::ObjectValue>();
		auto patch_obj = db_view->get_object(patch_obj_val->get_name());
		result.push_back(patch_obj);
	}

	return result;
}

} // namespace openage::gamestate::api
