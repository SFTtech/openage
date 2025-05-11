// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "ability.h"

#include <deque>
#include <memory>
#include <string>
#include <unordered_map>

#include <nyan/nyan.h>

#include "datastructure/constexpr_map.h"
#include "gamestate/api/definitions.h"


namespace openage::gamestate::api {

bool APIAbility::is_ability(const nyan::Object &obj) {
	for (auto &parent : obj.get_parents()) {
		if (parent == "engine.ability.Ability") {
			return true;
		}
	}

	return false;
}

bool APIAbility::check_type(const nyan::Object &ability,
                            const ability_t &type) {
	nyan::fqon_t immediate_parent = ability.get_parents()[0];
	nyan::ValueHolder ability_type = ABILITY_DEFS.get(type);

	std::shared_ptr<nyan::ObjectValue> ability_val = std::dynamic_pointer_cast<nyan::ObjectValue>(
		ability_type.get_ptr());

	return ability_val->get_name() == immediate_parent;
}

ability_t APIAbility::get_type(const nyan::Object &ability) {
	nyan::fqon_t immediate_parent = ability.get_parents()[0];

	// TODO: remove once other ability types are implemented
	if (not ABILITY_TYPE_LOOKUP.contains(immediate_parent)) {
		return ability_t::UNKNOWN;
	}

	return ABILITY_TYPE_LOOKUP.get(immediate_parent);
}

component::component_t APIAbility::get_component_type(const nyan::Object &ability) {
	auto ability_type = APIAbility::get_type(ability);

	return COMPONENT_TYPE_LOOKUP.get(ability_type);
}

bool APIAbility::check_property(const nyan::Object &ability, const ability_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = ability.get<nyan::Dict>("Ability.properties");
	nyan::ValueHolder property_type = ABILITY_PROPERTY_DEFS.get(property);

	return properties->contains(property_type);
}


const nyan::Object APIAbility::get_property(const nyan::Object &ability, const ability_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = ability.get<nyan::Dict>("Ability.properties");
	nyan::ValueHolder property_type = ABILITY_PROPERTY_DEFS.get(property);

	std::shared_ptr<nyan::View> db_view = ability.get_view();
	std::shared_ptr<nyan::ObjectValue> property_val = std::dynamic_pointer_cast<nyan::ObjectValue>(
		properties->get().at(property_type).get_ptr());

	return db_view->get_object(property_val->get_name());
}


} // namespace openage::gamestate::api
