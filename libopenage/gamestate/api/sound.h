// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <vector>

#include <nyan/nyan.h>


namespace openage::gamestate::api {

/**
 * Helper class for API sounds.
 */
class APISound {
public:
	/**
	 * Check if a nyan object is a sound (type == \p engine.util.sound.Sound).
	 *
	 * @param obj nyan object handle.
	 *
	 * @return true if the object is a sound, else false.
	 */
	static bool is_sound(const nyan::Object &obj);

	/**
	 * Get the sound path of a sound.
	 *
	 * The path is relative to the directory the modpack is mounted in.
	 *
	 * @param sound \p Sound nyan object (type == \p engine.util.sound.Sound).
	 *
	 * @return Relative path to the sound file.
	 */
	static const std::string get_sound_path(const nyan::Object &sound);

	/**
	 * Get the sound paths for a collection of sounds.
	 *
	 * Paths are relative to the directory the modpack is mounted in.
	 *
	 * @param sounds \p Sound nyan objects (type == \p engine.util.sound.Sound).
	 *
	 * @return Relative paths to the sound files.
	 */
	static const std::vector<std::string> get_sound_paths(const std::vector<nyan::Object> &sounds);
};
} // namespace openage::gamestate::api
