// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "condition_min_ressources.h"

namespace openage {

	ConditionMinRessources::ConditionMinRessources(uint32_t player,game_resource resource, float value) {
		this->player   = player;
		this->resource = resource;
		this->value    = value;
	}

	/*
	 * load from savegame
	 */
	ConditionMinRessources::ConditionMinRessources(Json::Value condition) {
	  	this->player   = condition.get("player",0).asUInt();
		this->resource = Resource::getResourceEnum( condition.get("resource","unknown").asString() );
		this->value    = condition.get("value",0).asFloat();
	}

	ConditionMinRessources::~ConditionMinRessources() {

	}

	bool ConditionMinRessources::check(uint32_t,uint32_t) {
		if( this->game->get_player(this->player)->amount(this->resource) >= this->value ) {
			return true;
		}
		return false;
	}

	/*
	 * create savegame
	 */
	Json::Value ConditionMinRessources::toJson() {
		Json::Value json;
		json["type"]     = "min-resources";
		json["player"]   = this->player;
		json["value"]    = this->value;
		json["resource"] = Resource::getResourceString(this->resource);
		return json;
	}
}
