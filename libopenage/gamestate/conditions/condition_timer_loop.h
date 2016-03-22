// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../condition.h"

namespace openage {

	class ConditionTimerLoop : public virtual Condition{
		public:
			ConditionTimerLoop(uint32_t ms);
			~ConditionTimerLoop();

			bool check(uint32_t gametime,uint32_t update);
			Json::Value toJson();

			uint32_t ms   = 0;
		private:
			uint32_t last = 0;

	};

}
