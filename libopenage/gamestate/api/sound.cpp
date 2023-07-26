// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "sound.h"

#include <deque>

#include <nyan/nyan.h>

#include "gamestate/api/util.h"


namespace openage::gamestate::api {

bool APISound::is_sound(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.util.sound.Sound";
}

const std::string APISound::get_sound_path(const nyan::Object &sound) {
	return resolve_file_path(sound, sound.get_file("Sound.sound"));
}

const std::vector<std::string> APISound::get_sound_paths(const std::vector<nyan::Object> &sounds) {
	std::vector<std::string> result;

	for (auto &sound : sounds) {
		result.push_back(get_sound_path(sound));
	}

	return result;
}

} // namespace openage::gamestate::api
