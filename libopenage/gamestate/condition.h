#pragma once

#include <cstdint>

#include "../engine.h"

namespace openage {

	class Condition {
		public:
			Condition();
			virtual ~Condition() {};
			virtual bool check(uint32_t gametime,uint32_t update) = 0;
			openage::GameMain* game;
	};
}

