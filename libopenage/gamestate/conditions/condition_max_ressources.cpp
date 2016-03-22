// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "condition_max_ressources.h"

namespace openage {

	ConditionMaxRessources::ConditionMaxRessources(uint32_t player,game_resource resource, float value) {
		this->player   = player;
		this->resource = resource;
		this->value    = value;
	}

	ConditionMaxRessources::~ConditionMaxRessources() {

	}

	bool ConditionMaxRessources::check(uint32_t gametime,uint32_t update) {
		if( this->game->get_player(this->player)->amount(this->resource) <= this->value ) {
			return true;
		}
		return false;
	}

	Json::Value ConditionMaxRessources::toJson() {
		Json::Value json;
		json["type"]     = "max-resources";
		json["player"]   = (double) this->player;
		json["value"]    = this->value;
		json["resource"] = this->getResourceString(this->resource);
		return json;
	}
}
