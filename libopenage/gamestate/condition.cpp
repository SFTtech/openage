#include <iostream>

#include "condition.h"

namespace openage {

	Condition::Condition() {
		this->game = Engine::get().get_game();
	}

	Condition::~Condition() {

	}
}
