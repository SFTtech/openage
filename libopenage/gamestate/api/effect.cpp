// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include "effect.h"

#include "gamestate/api/definitions.h"
#include "gamestate/api/util.h"


namespace openage::gamestate::api {

bool APIEffect::is_effect(const nyan::Object &obj) {
	return obj.extends("engine.effect.Effect");
}

bool APIEffect::check_type(const nyan::Object &effect,
                           const effect_t &type) {
	nyan::fqon_t api_parent = get_api_parent(effect);
	nyan::ValueHolder effect_type = EFFECT_DEFS.get(type);

	auto effect_val = effect_type.get_value_ptr<nyan::ObjectValue>();

	return effect_val->get_name() == api_parent;
}

bool APIEffect::check_property(const nyan::Object &effect,
                               const effect_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = effect.get<nyan::Dict>("Effect.properties");
	nyan::ValueHolder property_type = EFFECT_PROPERTY_DEFS.get(property);

	return properties->contains(property_type);
}

effect_t APIEffect::get_type(const nyan::Object &effect) {
	nyan::fqon_t api_parent = get_api_parent(effect);

	return EFFECT_TYPE_LOOKUP.get(api_parent);
}

const nyan::Object APIEffect::get_property(const nyan::Object &effect,
                                           const effect_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = effect.get<nyan::Dict>("Effect.properties");
	nyan::ValueHolder property_type = EFFECT_PROPERTY_DEFS.get(property);

	std::shared_ptr<nyan::View> db_view = effect.get_view();
	auto property_val = properties->get().at(property_type).get_value_ptr<nyan::ObjectValue>();

	return db_view->get_object(property_val->get_name());
}

} // namespace openage::gamestate::api
