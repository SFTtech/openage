// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include <nyan/nyan.h>


namespace openage::gamestate::api {

/**
 * Helper class for API player setups (== civs from AoE).
 */
class APIPlayerSetup {
public:
	/**
	 * Check if a nyan object is a player setup (type == \p engine.util.setup.PlayerSetup).
	 *
	 * @param obj nyan object handle.
	 *
	 * @return true if the object is a player setup, else false.
	 */
	static bool is_player_setup(const nyan::Object &obj);

	/**
	 * Get the modifiers of a player setup.
	 *
	 * @param player_setup nyan object (type == \p engine.util.setup.PlayerSetup).
	 *
	 * @return \p Modifier nyan objects (type == \p engine.modifier.Modifier).
	 */
	static const std::vector<nyan::Object> get_modifiers(const nyan::Object &player_setup);

	/**
	 * Get the starting resources of a player setup.
	 *
	 * @param player_setup nyan object (type == \p engine.util.setup.PlayerSetup).
	 *
	 * @return \p ResourceAmount nyan objects (type == \p engine.util.resource.ResourceAmount).
	 */
	static const std::vector<nyan::Object> get_start_resources(const nyan::Object &player_setup);

	/**
	 * Get the initial patches of a player setup.
	 *
	 * @param player_setup nyan object (type == \p engine.util.setup.PlayerSetup).
	 *
	 * @return \p Patch nyan objects (type == \p engine.util.patch.Patch).
	 */
	static const std::vector<nyan::Object> get_patches(const nyan::Object &player_setup);
};

} // namespace openage::gamestate::api
