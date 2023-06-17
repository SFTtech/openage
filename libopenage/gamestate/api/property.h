// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <nyan/nyan.h>


namespace openage::gamestate::api {

/**
 * Helper class for API properties.
 */
class APIProperty {
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
};

} // namespace openage::gamestate::api
