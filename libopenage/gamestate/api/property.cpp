// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "property.h"


namespace openage::gamestate::api {

bool APIProperty::is_property(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.ability.property.Property";
}

const std::vector<nyan::Object> APIProperty::get_animations(const nyan::Object &property) {
	std::vector<nyan::Object> ret;

	auto db_view = property.get_view();
	auto animations = property.get_set("Animated.animations");
	for (auto &anim_val : animations) {
		auto anim_obj_val = std::dynamic_pointer_cast<nyan::ObjectValue>(anim_val.get_ptr());
		auto anim_obj = db_view->get_object(anim_obj_val->get_name());
		ret.push_back(anim_obj);
	}

	return ret;
}


} // namespace openage::gamestate::api
