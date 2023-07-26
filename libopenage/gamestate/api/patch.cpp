// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "patch.h"

#include <deque>
#include <memory>
#include <string>
#include <unordered_map>

#include <nyan/nyan.h>

#include "datastructure/constexpr_map.h"
#include "gamestate/api/definitions.h"


namespace openage::gamestate::api {

bool APIPatch::is_patch(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.util.patch.Patch";
}

bool APIPatch::check_property(const nyan::Object &patch,
                              const patch_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = patch.get<nyan::Dict>("Patch.properties");
	nyan::ValueHolder property_type = PATCH_PROPERTY_DEFS.get(property);

	if (properties->contains(property_type)) {
		return true;
	}

	return false;
}

const nyan::Object APIPatch::get_property(const nyan::Object &patch,
                                          const patch_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = patch.get<nyan::Dict>("Patch.properties");
	nyan::ValueHolder property_type = PATCH_PROPERTY_DEFS.get(property);

	std::shared_ptr<nyan::View> db_view = patch.get_view();
	std::shared_ptr<nyan::ObjectValue> property_val = std::dynamic_pointer_cast<nyan::ObjectValue>(
		properties->get().at(property_type).get_ptr());

	return db_view->get_object(property_val->get_name());
}


} // namespace openage::gamestate::api
