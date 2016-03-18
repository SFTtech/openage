#include "action_add_gold.h"
#include "../game_main.h"

namespace openage {

	ActionAddGold::ActionAddGold() {

	}

	ActionAddGold::~ActionAddGold() {

	}

	// default action
	void ActionAddGold::execute() {

		if(1.0 > 0) {
			this->game->get_player(1)->receive(game_resource::gold,0.77);
			return;
		}
	}

}
