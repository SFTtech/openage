#include "../log/log.h"
#include "../engine.h"
#include "action.h"
#include "game_main.h"

namespace openage {

	Action::Action() {
		this->game = Engine::get().get_game();
	}

	Action::~Action() {

	}

	// default action

	void Action::execute() {
	  std::cout << "action" << std::flush;
		log::log(MSG(dbg) << "implement me!! action::execute");
	}


}
