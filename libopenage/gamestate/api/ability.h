// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <nyan/nyan.h>

#include "gamestate/api/types.h"

namespace openage::gamestate::api {

/**
 * Helper class for extracting values from Ability objects in the nyan API.
 */
class APIAbility {
public:
	/**
	 * Check if a nyan object is an Ability (type == \p engine.ability.Ability).
	 *
	 * @param obj nyan object.
	 *
	 * @return true if the object is an ability, else false.
	 */
	static bool is_ability(const nyan::Object &obj);

	/**
	 * Check if an ability has a given property.
	 *
	 * @param ability \p Ability nyan object (type == \p engine.ability.Ability).
	 * @param property Property type.
	 *
	 * @return true if the ability has the property, else false.
	 */
	static bool check_property(const nyan::Object &ability,
	                           const ability_property_t &property);

	/**
	 * Get the nyan object for a property from an ability.
	 *
	 * @param ability \p Ability nyan object (type == \p engine.ability.Ability).
	 * @param property Property type.
	 *
	 * @return \p Property nyan object (type == \p engine.ability.property.Property).
	 */
	static const nyan::Object get_property(const nyan::Object &ability,
	                                       const ability_property_t &property);
};

} // namespace openage::gamestate::api
