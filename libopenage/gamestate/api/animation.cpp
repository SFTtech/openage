// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "animation.h"

#include <deque>

#include "gamestate/api/util.h"


namespace openage::gamestate::api {

bool APIAnimation::is_animation(nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.ability.property.Property";
}

const std::string APIAnimation::get_animation_path(const nyan::Object &animation) {
	return resolve_file_path(animation, animation.get_file("Animation.sprite"));
}

const std::vector<std::string> APIAnimation::get_animation_paths(const std::vector<nyan::Object> &animations) {
	std::vector<std::string> result;

	for (auto &animation : animations) {
		result.push_back(get_animation_path(animation));
	}

	return result;
}

} // namespace openage::gamestate::api
