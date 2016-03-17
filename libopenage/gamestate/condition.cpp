#include <iostream>

#include "condition.h"

namespace openage {

	Condition::Condition() {

	}

	Condition::~Condition() {

	}

	bool Condition::check(uint32_t gametime,uint32_t update) {
		std::cout << update << std::endl << std::flush;
		return true;
	}

}
