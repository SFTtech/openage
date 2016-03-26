// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "condition_max_ressources.h"

namespace openage {

	ConditionMaxRessources::ConditionMaxRessources(uint32_t player,game_resource resource, float value) {
		this->player   = player;
		this->resource = resource;
		this->value    = value;
	}

	/*
	 * load from savegame
	 */
	ConditionMaxRessources::ConditionMaxRessources(Json::Value condition) {
	  	this->player   = condition.get("player",0).asUInt();
		this->resource = Resource::getResourceEnum( condition.get("resource","unkwown").asString() );
		this->value    = condition.get("value",0).asFloat();;
	}

	ConditionMaxRessources::~ConditionMaxRessources() {

	}

	bool ConditionMaxRessources::check(uint32_t,uint32_t) {
		if( this->game->get_player(this->player)->amount(this->resource) <= this->value ) {
			return true;
		}
		return false;
	}

	/*
	 * create savegame
	 */
	Json::Value ConditionMaxRessources::toJson() {
		Json::Value json;
		json["type"]     = "max-resources";
		json["player"]   = this->player;
		json["value"]    = this->value;
		json["resource"] = Resource::getResourceString(this->resource);
		return json;
	}
}
