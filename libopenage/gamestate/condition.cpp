// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <iostream>

#include "condition.h"

namespace openage {

	Condition::Condition() {
		this->game = Engine::get().get_game();
	}
}
