// Copyright 2023-2023 the openage authors. See copying.md for legal info.

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
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.ability.Ability";
}

bool APIAbility::check_property(const nyan::Object &ability, const ability_property_t &property) {
	std::shared_ptr<nyan::Dict> properties = ability.get<nyan::Dict>("Ability.properties");
	nyan::ValueHolder property_type = ABILITY_PROPERTY_DEFS.get(property);

	if (properties->contains(property_type)) {
		return true;
	}

	return false;
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
