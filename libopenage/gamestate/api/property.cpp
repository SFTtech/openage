// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "property.h"

#include <deque>
#include <memory>
#include <string>
#include <unordered_set>

#include <nyan/nyan.h>


namespace openage::gamestate::api {

bool APIAbilityProperty::is_property(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.ability.property.Property";
}

const std::vector<nyan::Object> APIAbilityProperty::get_animations(const nyan::Object &property) {
	std::vector<nyan::Object> result;

	auto db_view = property.get_view();
	auto animations = property.get_set("Animated.animations");
	for (auto &anim_val : animations) {
		auto anim_obj_val = std::dynamic_pointer_cast<nyan::ObjectValue>(anim_val.get_ptr());
		auto anim_obj = db_view->get_object(anim_obj_val->get_name());
		result.push_back(anim_obj);
	}

	return result;
}

const std::vector<nyan::Object> APIAbilityProperty::get_command_sounds(const nyan::Object &property) {
	std::vector<nyan::Object> result;

	auto db_view = property.get_view();
	auto command_sounds = property.get_set("CommandSound.sounds");
	for (auto &sound_val : command_sounds) {
		auto sound_obj_val = std::dynamic_pointer_cast<nyan::ObjectValue>(sound_val.get_ptr());
		auto sound_obj = db_view->get_object(sound_obj_val->get_name());
		result.push_back(sound_obj);
	}

	return result;
}

const std::vector<nyan::Object> APIAbilityProperty::get_execution_sounds(const nyan::Object &property) {
	std::vector<nyan::Object> result;

	auto db_view = property.get_view();
	auto execution_sounds = property.get_set("ExecutionSound.sounds");
	for (auto &sound_val : execution_sounds) {
		auto sound_obj_val = std::dynamic_pointer_cast<nyan::ObjectValue>(sound_val.get_ptr());
		auto sound_obj = db_view->get_object(sound_obj_val->get_name());
		result.push_back(sound_obj);
	}

	return result;
}

const std::vector<nyan::Object> APIAbilityProperty::get_diplo_stances(const nyan::Object &property) {
	std::vector<nyan::Object> result;

	auto db_view = property.get_view();
	auto diplo_stances = property.get_set("Diplomatic.stances");
	for (auto &diplo_stance_val : diplo_stances) {
		auto diplo_stance_obj_val = std::dynamic_pointer_cast<nyan::ObjectValue>(diplo_stance_val.get_ptr());
		auto diplo_stance_obj = db_view->get_object(diplo_stance_obj_val->get_name());
		result.push_back(diplo_stance_obj);
	}

	return result;
}

} // namespace openage::gamestate::api
