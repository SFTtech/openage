// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../condition.h"
#include "../picojson.h"

namespace openage {

	class ConditionEveryTick : public virtual Condition{
		public:
			ConditionEveryTick();
			~ConditionEveryTick();

			bool check(uint32_t gametime,uint32_t update);
			Json::Value toJson();

	};

}
