// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <nyan/nyan.h>

#include "gamestate/api/types.h"


namespace openage::gamestate::api {

/**
 * Helper class for extracting values from Effect objects in the nyan API.
 */
class APIEffect {
public:
	/**
	 * Check if a nyan object is an Effect (type == \p engine.effect.Effect).
	 *
	 * @param obj nyan object.
	 *
	 * @return true if the object is an effect, else false.
	 */
	static bool is_effect(const nyan::Object &obj);

	/**
	 * Check if an effect is of a given type.
	 *
	 * @param effect \p Effect nyan object (type == \p engine.effect.Effect).
	 * @param type Effect type.
	 *
	 * @return true if the effect is of the given type, else false.
	 */
	static bool check_type(const nyan::Object &effect,
	                       const effect_t &type);

	/**
	 * Check if an effect has a given property.
	 *
	 * @param effect \p Effect nyan object (type == \p engine.effect.Effect).
	 * @param property Property type.
	 *
	 * @return true if the effect has the property, else false.
	 */
	static bool check_property(const nyan::Object &effect,
	                           const effect_property_t &property);

	/**
	 * Get the type of an effect.
	 *
	 * @param effect \p Effect nyan object (type == \p engine.effect.Effect).
	 *
	 * @return Type of the effect.
	 */
	static effect_t get_type(const nyan::Object &effect);

	/**
	 * Get the nyan object for a property from an effect.
	 *
	 * @param effect \p Effect nyan object (type == \p engine.effect.Effect).
	 * @param property Property type.
	 *
	 * @return \p Property nyan object (type == \p engine.effect.property.Property).
	 */
	static const nyan::Object get_property(const nyan::Object &effect,
	                                       const effect_property_t &property);
};

} // namespace openage::gamestate::api
