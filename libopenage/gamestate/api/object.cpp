// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#include "object.h"

#include "error/error.h"


namespace openage::gamestate::api {

bool APIObject::is_object(const nyan::Object &obj) {
	return obj.extends("engine.root.Object");
}

} // namespace openage::gamestate::api
