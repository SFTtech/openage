// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include "resistance.h"

#include "gamestate/api/definitions.h"
#include "gamestate/api/util.h"


namespace openage::gamestate::api {

bool APIResistance::is_resistance(const nyan::Object &obj) {
	for (auto &parent : obj.get_parents()) {
		if (parent == "engine.resistance.Resistance") {
			return true;
		}
	}

	return false;
}

bool APIResistance::check_effect_type(const nyan::Object &resistance,
                                      const effect_t &type) {
	nyan::fqon_t api_parent = get_api_parent(resistance);
	nyan::ValueHolder effect_type = RESISTANCE_DEFS.get(type);

	std::shared_ptr<nyan::ObjectValue> effect_val = std::dynamic_pointer_cast<nyan::ObjectValue>(
		effect_type.get_ptr());

	return effect_val->get_name() == api_parent;
}

bool APIResistance::check_property(const nyan::Object &resistance,
                                   const resistance_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = resistance.get<nyan::Dict>("Resistance.properties");
	nyan::ValueHolder property_type = RESISTANCE_PROPERTY_DEFS.get(property);

	return properties->contains(property_type);
}

effect_t APIResistance::get_effect_type(const nyan::Object &resistance) {
	nyan::fqon_t api_parent = get_api_parent(resistance);

	return RESISTANCE_TYPE_LOOKUP.get(api_parent);
}

const nyan::Object APIResistance::get_property(const nyan::Object &resistance,
                                               const resistance_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = resistance.get<nyan::Dict>("Resistance.properties");
	nyan::ValueHolder property_type = RESISTANCE_PROPERTY_DEFS.get(property);

	std::shared_ptr<nyan::View> db_view = resistance.get_view();
	std::shared_ptr<nyan::ObjectValue> property_val = std::dynamic_pointer_cast<nyan::ObjectValue>(
		properties->get().at(property_type).get_ptr());

	return db_view->get_object(property_val->get_name());
}

} // namespace openage::gamestate::api
