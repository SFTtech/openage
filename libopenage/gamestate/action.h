#ifndef ACTION_H
#define ACTION_H

#include "game_main.h"

namespace openage {

	class Action {
		public:
			Action();
			~Action();
			virtual void execute();
			openage::GameMain* game;
	};

}

#endif // ACTION_H
