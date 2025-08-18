// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <nyan/nyan.h>


namespace nyan {
class Object;
} // namespace nyan


namespace openage::gamestate::api {

/**
 * Helper class for getting info on generic objects in the nyan API.
 */
class APIObject {
public:
    /**
     * Check if a nyan object is an API Object (type == \p engine.root.Object).
     *
     * @param obj nyan object.
     *
     * @return true if the object is an object, else false.
     */
	static bool is_object(const nyan::Object &obj);
};

} // namespace
