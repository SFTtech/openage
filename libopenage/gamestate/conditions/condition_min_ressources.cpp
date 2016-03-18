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

}
