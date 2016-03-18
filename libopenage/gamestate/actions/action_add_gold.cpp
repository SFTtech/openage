#include "action_add_gold.h"
#include "../game_main.h"

namespace openage {

	ActionAddGold::ActionAddGold(float amount,uint16_t player) {
		this->amount = amount;
		this->player = player;
	}

	ActionAddGold::~ActionAddGold() {

	}

	// default action
	void ActionAddGold::execute() {
		if(this->amount > 0) {
			this->game->get_player(this->player)->receive(game_resource::gold,this->amount);
		} else {
			this->game->get_player(this->player)->deduct(game_resource::gold,this->amount);
		}
	}

}
