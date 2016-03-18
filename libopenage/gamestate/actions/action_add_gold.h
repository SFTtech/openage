#pragma once

#include "../action.h"

namespace openage {

	class ActionAddGold : public virtual Action{
		public:
			ActionAddGold(float amount,uint16_t player);
			~ActionAddGold();
			void execute();

			float    amount = 0;
			uint16_t player = 0;
	};

}

