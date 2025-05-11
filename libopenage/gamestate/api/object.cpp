// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#include "object.h"

#include "error/error.h"


namespace openage::gamestate::api {

bool APIObject::is_object(const nyan::Object &obj) {
    for (const auto &parent : obj.get_parents()) {
        if (parent == "engine.root.Object") {
            return true;
        }
    }

    return false;
}

} // namespace
