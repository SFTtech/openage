// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <nyan/nyan.h>

#include "gamestate/api/types.h"


namespace openage::gamestate::api {

/**
 * Helper class for API patches.
 */
class APIPatch {
public:
	/**
	 * Check if a nyan object is a patch (type == \p engine.util.patch.Patch).
	 *
	 * @param obj nyan object handle.
	 *
	 * @return true if the object is a patch, else false.
	 */
	static bool is_patch(const nyan::Object &obj);

	/**
	 * Check if a patch has a given property.
	 *
	 * @param patch \p Patch nyan object (type == \p engine.util.patch.Patch).
	 * @param property Property type.
	 *
	 * @return true if the patch has the property, else false.
	 */
	static bool check_property(const nyan::Object &patch,
	                           const patch_property_t &property);

	/**
	 * Get the nyan object for a property from a patch.
	 *
	 * @param patch \p Patch nyan object (type == \p engine.util.patch.Patch).
	 * @param property Property type.
	 *
	 * @return \p Property nyan object (type == \p engine.util.patch.property.PatchProperty).
	 */
	static const nyan::Object get_property(const nyan::Object &patch,
	                                       const patch_property_t &property);
};
} // namespace openage::gamestate::api
