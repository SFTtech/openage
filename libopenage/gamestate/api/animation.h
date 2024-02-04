// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <vector>

#include <nyan/nyan.h>


namespace openage::gamestate::api {

/**
 * Helper class for API animations.
 */
class APIAnimation {
public:
	/**
	 * Check if a nyan object is an animation (type == \p engine.util.animation.Animation).
	 *
	 * @param obj nyan object handle.
	 *
	 * @return true if the object is an animation, else false.
	 */
	static bool is_animation(nyan::Object &obj);

	/**
	 * Get the sprite path of an animation.
	 *
	 * The path is relative to the directory the modpack is mounted in.
	 *
	 * @param animation \p Animation nyan object (type == \p engine.util.animation.Animation).
	 *
	 * @return Relative path to the animation sprite file.
	 */
	static const std::string get_animation_path(const nyan::Object &animation);

	/**
	 * Get the sprite paths for a collection of animations.
	 *
	 * Paths are relative to the directory the modpack is mounted in.
	 *
	 * @param animations \p Animation nyan objects (type == \p engine.util.animation.Animation).
	 *
	 * @return Relative paths to the animation sprite files.
	 */
	static const std::vector<std::string> get_animation_paths(const std::vector<nyan::Object> &animations);
};

} // namespace openage::gamestate::api
