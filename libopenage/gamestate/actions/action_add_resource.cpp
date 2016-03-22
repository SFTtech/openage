// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "action_add_resource.h"
#include "../game_main.h"
#include "../../engine.h"

namespace openage {

	ActionAddResource::ActionAddResource(float amount,uint16_t player, game_resource resource) {
		this->amount = amount;
		this->player = player;
		this->resource = resource;
		this->game     = Engine::get().get_game();
	}

	ActionAddResource::ActionAddResource(Json::Value action) {
	  /*
		ActionAddResource(
			action.get("amount").asFloat(),
			action.get("player").asUInt(),
			game action.get("resource").asString()
		);*/
	}

	ActionAddResource::~ActionAddResource() {

	}

	// default action
	void ActionAddResource::execute() {
		if(this->amount > 0) {
			this->game->get_player(this->player)->receive(this->resource,this->amount);
		} else {
			this->game->get_player(this->player)->deduct(this->resource,this->amount);
		}
	}

	//to Json
	Json::Value ActionAddResource::toJson() {
		Json::Value json;
		json["type"]     = "add-"+this->getResourceString(this->resource);
		json["amount"]   = this->amount;
		json["player"]   = this->player;
		json["resource"] = this->getResourceString(this->resource);
		return json;
	}

}
