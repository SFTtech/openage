// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include <nyan/nyan.h>


namespace openage::gamestate::api {

/**
 * Helper class for API properties.
 */
class APIAbilityProperty {
public:
	/**
	 * Check if a nyan object is a property (type == \p engine.ability.property.Property).
	 *
	 * @param obj nyan object handle.
	 *
	 * @return true if the object is a property, else false.
	 */
	static bool is_property(const nyan::Object &obj);

	/**
	 * Get the animations of an \p Animated property (type == \p engine.ability.property.type.Animated).
	 *
	 * @param property \p Property nyan object (type == \p engine.ability.property.Property).
	 *
	 * @return \p Animation nyan objects (type == \p engine.util.animation.Animation).
	 */
	static const std::vector<nyan::Object> get_animations(const nyan::Object &property);

	/**
	 * Get the sounds of a \p CommandSound property (type == \p engine.ability.property.type.CommandSound).
	 *
	 * @param property \p Property nyan object (type == \p engine.ability.property.Property).
	 *
	 * @return \p Sound nyan objects (type == \p engine.util.sound.Sound).
	 */
	static const std::vector<nyan::Object> get_command_sounds(const nyan::Object &property);

	/**
	 * Get the sounds of an \p ExecutionSound property (type == \p engine.ability.property.type.ExecutionSound).
	 *
	 * @param property \p Property nyan object (type == \p engine.ability.property.Property).
	 *
	 * @return \p Sound nyan objects (type == \p engine.util.sound.Sound).
	 */
	static const std::vector<nyan::Object> get_execution_sounds(const nyan::Object &property);

	/**
	 * Get the sounds of a \p Diplomatic property (type == \p engine.ability.property.type.Diplomatic).
	 *
	 * @param property \p Property nyan object (type == \p engine.ability.property.Property).
	 *
	 * @return \p DiplomaticStance nyan objects (type == \p engine.util.diplomatic_stance.DiplomaticStance).
	 */
	static const std::vector<nyan::Object> get_diplo_stances(const nyan::Object &property);
};

} // namespace openage::gamestate::api
