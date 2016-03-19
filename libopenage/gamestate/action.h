#pragma once

#include "game_main.h"

namespace openage {

	class Action {
		public:
			Action() {};
			virtual ~Action() {};
			virtual void execute() = 0;
			openage::GameMain* game;
	};

}
