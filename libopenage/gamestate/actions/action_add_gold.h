#pragma once

#include "../action.h"

namespace openage {

	class ActionAddGold : public Action{
		public:
			ActionAddGold();
			~ActionAddGold();
			void execute();

		 private:
			float amount;
	};

}

