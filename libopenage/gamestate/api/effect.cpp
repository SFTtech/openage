// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "effect.h"

#include "gamestate/api/definitions.h"


namespace openage::gamestate::api {

bool APIEffect::is_effect(const nyan::Object &obj) {
	for (auto &parent : obj.get_parents()) {
		if (parent == "engine.effect.Effect") {
			return true;
		}
	}

	return false;
}

bool APIEffect::check_type(const nyan::Object &effect,
                           const effect_t &type) {
	nyan::fqon_t immediate_parent = effect.get_parents()[0];
	nyan::ValueHolder effect_type = EFFECT_DEFS.get(type);

	std::shared_ptr<nyan::ObjectValue> effect_val = std::dynamic_pointer_cast<nyan::ObjectValue>(
		effect_type.get_ptr());

	return effect_val->get_name() == immediate_parent;
}

bool APIEffect::check_property(const nyan::Object &effect,
                               const effect_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = effect.get<nyan::Dict>("Effect.properties");
	nyan::ValueHolder property_type = EFFECT_PROPERTY_DEFS.get(property);

	return properties->contains(property_type);
}

effect_t APIEffect::get_type(const nyan::Object &effect) {
	nyan::fqon_t immediate_parent = effect.get_parents()[0];

	return EFFECT_TYPE_LOOKUP.get(immediate_parent);
}

const nyan::Object APIEffect::get_property(const nyan::Object &effect,
                                           const effect_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = effect.get<nyan::Dict>("Effect.properties");
	nyan::ValueHolder property_type = EFFECT_PROPERTY_DEFS.get(property);

	std::shared_ptr<nyan::View> db_view = effect.get_view();
	std::shared_ptr<nyan::ObjectValue> property_val = std::dynamic_pointer_cast<nyan::ObjectValue>(
		properties->get().at(property_type).get_ptr());

	return db_view->get_object(property_val->get_name());
}

} // namespace openage::gamestate::api
