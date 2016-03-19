#include "condition_min_ressources.h"

namespace openage {

	ConditionMinRessources::ConditionMinRessources(uint32_t player,game_resource resource, float value) {
		this->player   = player;
		this->resource = resource;
		this->value    = value;
	}

	ConditionMinRessources::~ConditionMinRessources() {

	}

	bool ConditionMinRessources::check(uint32_t gametime,uint32_t update) {
		if( this->game->get_player(this->player)->amount(this->resource) >= this->value ) {
			return true;
		}
		return false;
	}

}
