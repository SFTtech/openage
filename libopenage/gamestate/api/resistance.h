// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <nyan/nyan.h>

#include "gamestate/api/types.h"


namespace openage::gamestate::api {

/**
 * Helper class for extracting values from Resistance objects in the nyan API.
 */
class APIResistance {
public:
	/**
	 * Check if a nyan object is an Resistance (type == \p engine.resistance.Resistance).
	 *
	 * @param obj nyan object.
	 *
	 * @return true if the object is an resistance, else false.
	 */
	static bool is_resistance(const nyan::Object &obj);

	/**
	 * Check if an resistance matches a given effect type.
	 *
	 * @param resistance \p Resistance nyan object (type == \p engine.resistance.Resistance).
	 * @param type Effect type.
	 *
	 * @return true if the resistance is of the given type, else false.
	 */
	static bool check_effect_type(const nyan::Object &resistance,
	                              const effect_t &type);

	/**
	 * Check if an resistance has a given property.
	 *
	 * @param resistance \p Resistance nyan object (type == \p engine.resistance.Resistance).
	 * @param property Property type.
	 *
	 * @return true if the resistance has the property, else false.
	 */
	static bool check_property(const nyan::Object &resistance,
	                           const resistance_property_t &property);

	/**
	 * Get the matching effect type of a resistance.
	 *
	 * @param resistance \p Resistance nyan object (type == \p engine.resistance.Resistance).
	 *
	 * @return Type of the resistance.
	 */
	static effect_t get_effect_type(const nyan::Object &resistance);

	/**
	 * Get the nyan object for a property from an resistance.
	 *
	 * @param resistance \p Resistance nyan object (type == \p engine.resistance.Resistance).
	 * @param property Property type.
	 *
	 * @return \p Property nyan object (type == \p engine.resistance.property.Property).
	 */
	static const nyan::Object get_property(const nyan::Object &resistance,
	                                       const resistance_property_t &property);
};

} // namespace openage::gamestate::api
